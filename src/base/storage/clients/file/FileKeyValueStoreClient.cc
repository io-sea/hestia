#include "FileKeyValueStoreClient.h"

#include "FileUtils.h"
#include "JsonUtils.h"

#include "Logger.h"

namespace hestia {
FileKeyValueStoreClient::FileKeyValueStoreClient() {}

void FileKeyValueStoreClient::initialize(const Metadata& config)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "root") {
            m_store = value;
        }
    };
    config.for_each_item(on_item);

    LOG_INFO("Initializing at: " + m_store.string());
}

bool FileKeyValueStoreClient::exists(const StorageObject& obj) const
{
    return std::filesystem::exists(get_filename(obj));
}

void FileKeyValueStoreClient::put(
    const StorageObject& obj, const std::vector<std::string>& keys) const
{
    (void)keys;
    const auto path = get_filename(obj);
    JsonUtils::write(path, obj.m_metadata, exists(obj));
}

void FileKeyValueStoreClient::get(
    StorageObject& obj, const std::vector<std::string>& keys) const
{
    const auto path = get_filename(obj);
    JsonUtils::read(path, obj.m_metadata, keys);
}

void FileKeyValueStoreClient::remove(const StorageObject& obj) const
{
    std::filesystem::remove(get_filename(obj));
}

void FileKeyValueStoreClient::list(
    const Metadata::Query& query, std::vector<StorageObject>& fetched) const
{
    (void)query;
    (void)fetched;

    for (const auto& dir_entry : std::filesystem::directory_iterator{m_store}) {
        if (FileUtils::is_file_with_extension(dir_entry, ".dat")) {
            Metadata metadata;
            JsonUtils::read(dir_entry.path(), metadata);
            /*
                    if (metadata.getItem("tiers") == std::to_string(tier))
                    {
                        hestia::StorageObject
               obj(hestia::FileUtils::getFilenameWithoutExtension(dir_entry.path()));
                        objects.push_back(obj);
                    }
            */
        }
    }
}

std::filesystem::path FileKeyValueStoreClient::get_filename(
    const StorageObject& obj) const
{
    const auto path = m_store / ("objects/" + obj.m_id + ".dat");
    return path.string();
}
}  // namespace hestia