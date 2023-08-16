#include "FileObjectStoreClient.h"

#include "FileStreamSink.h"
#include "FileStreamSource.h"

#include "FileUtils.h"
#include "Logger.h"
#include "ProjectConfig.h"
#include "StringUtils.h"

#include <exception>
#include <fstream>
#include <string>

namespace hestia {

FileObjectStoreClient::Ptr FileObjectStoreClient::create()
{
    return std::make_unique<FileObjectStoreClient>();
}

std::string FileObjectStoreClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name()
           + "::FileObjectStoreClient";
}

void FileObjectStoreClient::initialize(
    const std::string& id,
    const std::string& cache_path,
    const Dictionary& config_data)
{
    FileObjectStoreClientConfig config;
    config.deserialize(config_data);
    do_initialize(id, cache_path, config);
}

void FileObjectStoreClient::do_initialize(
    const std::string& id,
    const std::string& cache_path,
    const FileObjectStoreClientConfig& config)
{
    m_id = id;

    m_root = config.m_root.get_value();
    m_mode = config.m_mode.get_value();

    if (m_root.is_relative()) {
        m_root = std::filesystem::path(cache_path) / m_root;
    }

    LOG_INFO("Initializing with root: " + m_root.string());

    hestia::FileUtils::create_if_not_existing(m_root);
}

void FileObjectStoreClient::remove(const StorageObject& object) const
{
    if (needs_metadata()) {
        std::filesystem::remove(get_metadata_path(object.id()));
    }

    if (needs_data()) {
        std::filesystem::remove(get_data_path(object.id()));
    }
}

bool FileObjectStoreClient::needs_metadata() const
{
    return m_mode == FileObjectStoreClientConfig::Mode::DATA_AND_METADATA
           || m_mode == FileObjectStoreClientConfig::Mode::METADATA_ONLY;
}

bool FileObjectStoreClient::needs_data() const
{
    return m_mode == FileObjectStoreClientConfig::Mode::DATA_ONLY
           || m_mode == FileObjectStoreClientConfig::Mode::DATA_AND_METADATA;
}

void FileObjectStoreClient::migrate(
    const std::string& object_id,
    const std::filesystem::path& root,
    bool keep_existing)
{
    if (!exists(object_id)) {
        throw std::runtime_error(
            "Couldn't find requested source object during migrate.");
    }

    const auto target_data_path     = get_data_path(object_id, root);
    const auto target_metadata_path = get_metadata_path(object_id, root);
    const auto source_data_path     = get_data_path(object_id);
    const auto source_metadata_path = get_metadata_path(object_id);

    if (needs_data()) {
        hestia::FileUtils::create_if_not_existing(target_data_path);
    }

    if (needs_metadata()) {
        hestia::FileUtils::create_if_not_existing(target_metadata_path);
    }

    if (keep_existing) {
        if (needs_metadata()) {
            std::filesystem::copy(
                source_metadata_path, target_metadata_path,
                std::filesystem::copy_options::overwrite_existing);
        }

        if (needs_data()) {
            if (std::filesystem::is_regular_file(source_data_path)) {
                std::filesystem::copy(
                    source_data_path, target_data_path,
                    std::filesystem::copy_options::overwrite_existing);
            }
        }
    }
    else {
        if (needs_metadata()) {
            std::filesystem::rename(source_metadata_path, target_metadata_path);
        }

        if (needs_data()) {
            if (std::filesystem::is_regular_file(source_data_path)) {
                std::filesystem::rename(source_data_path, target_data_path);
            }
        }
    }
}

bool FileObjectStoreClient::exists(const StorageObject& object) const
{
    return exists(object.id());
}

void FileObjectStoreClient::put(
    const StorageObject& object, const Extent&, Stream* stream) const
{
    if (needs_metadata()) {
        auto path = get_metadata_path(object.id());
        FileUtils::create_if_not_existing(path);

        std::ofstream meta_file(path);
        auto for_item =
            [&meta_file](const std::string& key, const std::string& value) {
                meta_file << key << " " << value << "\n";
            };
        object.metadata().for_each_item(for_item);
    }

    if (needs_data()) {
        LOG_INFO("Adding to sink: " << get_data_path(object.id()));
        if (stream != nullptr) {
            auto stream_sink =
                std::make_unique<FileStreamSink>(get_data_path(object.id()));
            stream->set_sink(std::move(stream_sink));
        }
    }
}

void FileObjectStoreClient::get(
    StorageObject& object, const Extent&, Stream* stream) const
{
    if (!exists(object)) {
        const std::string msg =
            "Requested object: " + object.id()
            + " not found in: " + get_data_path(object.id()).string();
        LOG_ERROR(msg);
        throw ObjectStoreException(
            {ObjectStoreErrorCode::OBJECT_NOT_FOUND, msg});
    }

    if (needs_metadata()) {
        read_metadata(object);
    }

    if (needs_data()) {
        if (stream != nullptr) {
            auto stream_source =
                std::make_unique<FileStreamSource>(get_data_path(object.id()));
            stream->set_source(std::move(stream_source));
        }
    }
}

bool FileObjectStoreClient::has_data(const std::string& object_id) const
{
    return std::filesystem::is_regular_file(get_data_path(object_id));
}

void FileObjectStoreClient::list(
    const KeyValuePair& query, std::vector<StorageObject>& objects) const
{
    for (const auto& dir_entry : std::filesystem::directory_iterator(m_root)) {
        if (FileUtils::is_file_with_extension(dir_entry, ".meta")) {
            const auto object_id =
                FileUtils::get_filename_without_extension(dir_entry.path());

            if (const auto value = get_metadata_item(object_id, query.first);
                value == query.second) {
                StorageObject object(object_id);
                read_metadata(object);
                objects.push_back(object);
            }
        }
    }
}

std::string FileObjectStoreClient::get_metadata_item(
    const std::string& object_id, const std::string& search_key) const
{
    std::ifstream md_file(get_metadata_path(object_id));
    std::string line;
    std::string value;
    while (std::getline(md_file, line)) {
        const auto& [key, val] = StringUtils::split_on_first(line, ' ');
        if (search_key == key) {
            value = val;
            break;
        }
    }
    return value;
}

void FileObjectStoreClient::read_metadata(StorageObject& object) const
{
    std::ifstream md_file(get_metadata_path(object.id()));
    std::pair<std::string, std::string> pair;
    while (md_file >> pair.first >> pair.second) {
        object.set_metadata(pair.first, pair.second);
    }
}

std::filesystem::path FileObjectStoreClient::get_data_path(
    const std::string& object_id, const std::filesystem::path& root) const
{
    return root.empty() ? m_root / (object_id + ".data") :
                          root / (object_id + ".data");
}

std::filesystem::path FileObjectStoreClient::get_metadata_path(
    const std::string& object_id, const std::filesystem::path& root) const
{
    return root.empty() ? m_root / (object_id + ".meta") :
                          root / (object_id + ".meta");
}

bool FileObjectStoreClient::exists(const std::string& object_id) const
{
    if (needs_metadata()) {
        return std::filesystem::is_regular_file(get_metadata_path(object_id));
    }
    else {
        return std::filesystem::is_regular_file(get_data_path(object_id));
    }
}
}  // namespace hestia
