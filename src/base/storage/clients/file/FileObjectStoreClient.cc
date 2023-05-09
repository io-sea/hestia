#include "FileObjectStoreClient.h"

#include "FileStreamSink.h"
#include "FileStreamSource.h"

#include "FileUtils.h"
#include "Logger.h"
#include "ProjectConfig.h"
#include "StringUtils.h"

#include <exception>
#include <fstream>

namespace hestia {

FileObjectStoreClient::Ptr FileObjectStoreClient::create()
{
    return std::make_unique<FileObjectStoreClient>();
}

FileObjectStoreClient::Ptr FileObjectStoreClient::create(
    const std::filesystem::path& root)
{
    auto instance = create();
    instance->do_initialize(root);
    return instance;
}

std::string FileObjectStoreClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name()
           + "::FileObjectStoreClient";
}

void FileObjectStoreClient::do_initialize(const std::filesystem::path& root)
{
    m_root = root;
    hestia::FileUtils::create_if_not_existing(m_root);
}

void FileObjectStoreClient::remove(const StorageObject& object) const
{
    std::filesystem::remove(get_metadata_path(object.m_id));
    std::filesystem::remove(get_data_path(object.m_id));
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
    hestia::FileUtils::create_if_not_existing(target_data_path);
    hestia::FileUtils::create_if_not_existing(target_metadata_path);

    const auto source_data_path     = get_data_path(object_id);
    const auto source_metadata_path = get_metadata_path(object_id);

    if (keep_existing) {
        std::filesystem::copy(source_metadata_path, target_metadata_path);
        if (std::filesystem::is_regular_file(source_data_path)) {
            std::filesystem::copy(source_data_path, target_data_path);
        }
    }
    else {
        std::filesystem::rename(source_metadata_path, target_metadata_path);
        if (std::filesystem::is_regular_file(source_data_path)) {
            std::filesystem::rename(source_data_path, target_data_path);
        }
    }
}

bool FileObjectStoreClient::exists(const StorageObject& object) const
{
    return exists(object.m_id);
}

void FileObjectStoreClient::put(
    const StorageObject& object, const Extent& extent, Stream* stream) const
{
    (void)extent;

    auto path = get_metadata_path(object.m_id);
    FileUtils::create_if_not_existing(path);

    std::ofstream meta_file(path);
    auto for_item =
        [&meta_file](const std::string& key, const std::string& value) {
            meta_file << key << " " << value << "\n";
        };
    object.m_metadata.for_each_item(for_item);

    if (stream != nullptr) {
        auto stream_sink =
            std::make_unique<FileStreamSink>(get_data_path(object.m_id));
        stream->set_sink(std::move(stream_sink));
    }
}

void FileObjectStoreClient::get(
    StorageObject& object, const Extent& extent, Stream* stream) const
{
    (void)extent;

    if (!exists(object)) {
        const std::string msg =
            "Requested object: " + object.m_id + " not found.";
        LOG_ERROR(msg);
        throw ObjectStoreException(
            {ObjectStoreErrorCode::OBJECT_NOT_FOUND, msg});
    }

    read_metadata(object);
    if (stream != nullptr) {
        auto stream_source =
            std::make_unique<FileStreamSource>(get_data_path(object.m_id));
        stream->set_source(std::move(stream_source));
    }
}

bool FileObjectStoreClient::has_data(const std::string& object_id) const
{
    return std::filesystem::is_regular_file(get_data_path(object_id));
}

void FileObjectStoreClient::list(
    const Metadata::Query& query, std::vector<StorageObject>& objects) const
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
    std::ifstream md_file(get_metadata_path(object.m_id));
    std::pair<std::string, std::string> pair;
    while (md_file >> pair.first >> pair.second) {
        object.m_metadata.set_item(pair.first, pair.second);
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
    return std::filesystem::is_regular_file(get_metadata_path(object_id));
}
}  // namespace hestia
