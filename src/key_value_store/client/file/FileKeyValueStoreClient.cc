#include "FileKeyValueStoreClient.h"

#include <ostk/FileUtils.h>
#include <ostk/JsonUtils.h>

FileKeyValueStoreClient::FileKeyValueStoreClient() {}

bool FileKeyValueStoreClient::exists(const ostk::StorageObject& obj) const
{
    return std::filesystem::exists(get_filename(obj));
}

void FileKeyValueStoreClient::put(
    const ostk::StorageObject& obj, const std::vector<std::string>& keys) const
{
    const auto path = get_filename(obj);
    ostk::JsonUtils::write(path, obj.m_metadata, exists(obj));
}

void FileKeyValueStoreClient::get(
    ostk::StorageObject& obj, const std::vector<std::string>& keys) const
{
    const auto path = get_filename(obj);
    ostk::JsonUtils::read(path, obj.m_metadata, keys);
}

void FileKeyValueStoreClient::remove(const ostk::StorageObject& obj) const
{
    std::filesystem::remove(get_filename(obj));
}

void FileKeyValueStoreClient::list(
    const ostk::Metadata::Query& query,
    std::vector<ostk::StorageObject>& fetched) const
{
    for (const auto& dir_entry :
         std::filesystem::directory_iterator{m_store.path()}) {
        if (ostk::FileUtils::is_file_with_extension(dir_entry, ".meta")) {
            ostk::Metadata metadata;
            ostk::JsonUtils::read(dir_entry.path(), metadata);
            /*
                    if (metadata.getItem("tiers") == std::to_string(tier))
                    {
                        ostk::StorageObject
               obj(ostk::FileUtils::getFilenameWithoutExtension(dir_entry.path()));
                        objects.push_back(obj);
                    }
            */
        }
    }
}

std::filesystem::path FileKeyValueStoreClient::get_filename(
    const ostk::StorageObject& obj) const
{
    const auto path = m_store.path() / (obj.m_id + ".meta");
    return path.string();
}