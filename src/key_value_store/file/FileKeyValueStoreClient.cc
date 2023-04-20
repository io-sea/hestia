#include "FileKeyValueStoreClient.h"

#include <ostk/JsonUtils.h>
#include <ostk/FileUtils.h>

FileKeyValueStoreClient::FileKeyValueStoreClient()
{

}

bool FileKeyValueStoreClient::exists(const ostk::StorageObject& obj) const
{
    return std::filesystem::exists(getFilename(obj));
}

void FileKeyValueStoreClient::put(const ostk::StorageObject& obj, const std::vector<std::string>& keys) const
{
    const auto path = getFilename(obj);
    ostk::JsonUtils::write(path, obj.mMetadata, exists(obj));
}

void FileKeyValueStoreClient::get(ostk::StorageObject& obj, const std::vector<std::string>& keys) const
{
    const auto path = getFilename(obj);
    ostk::JsonUtils::read(path, obj.mMetadata, keys);
}

void FileKeyValueStoreClient::remove(const ostk::StorageObject& obj) const
{
    std::filesystem::remove(getFilename(obj));
}

void FileKeyValueStoreClient::list(const ostk::Metadata::Query& query, std::vector<ostk::StorageObject>& fetched) const 
{
    for (const auto& dir_entry : std::filesystem::directory_iterator{mStore.path()}) 
    {
        if (ostk::FileUtils::isFileWithExtension(dir_entry, ".meta")) 
        {
            ostk::Metadata metadata;
            ostk::JsonUtils::read(dir_entry.path(), metadata);
    /*
            if (metadata.getItem("tiers") == std::to_string(tier))
            {
                ostk::StorageObject obj(ostk::FileUtils::getFilenameWithoutExtension(dir_entry.path()));
                objects.push_back(obj);
            }
    */
        }
    }
}

std::filesystem::path FileKeyValueStoreClient::getFilename(const ostk::StorageObject& obj) const
{
    const auto path = mStore.path() / (obj.mId + ".meta");
    return path.string();
}