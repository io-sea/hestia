#include "S3ObjectModel.h"

#include <iostream>

namespace hestia {
const std::array<std::string, 5> S3ObjectModel::internal_keys{
    "key", "creation_time", "content_length", "bucket", "protocol"};

Metadata S3ObjectModel::serialize(
    const S3Container& container,
    const S3Object& object,
    const std::string& prefix)
{
    Metadata metadata = object.m_user_metadata;
    metadata.set_item("key", object.m_key);
    metadata.set_item(prefix + ":creation_time", object.m_creation_time);
    metadata.set_item(prefix + ":content_length", object.m_content_length);
    metadata.set_item(prefix + ":bucket", container.m_name);
    metadata.set_item(prefix + ":protocol", "s3");
    return metadata;
}

std::string S3ObjectModel::json_serialize(
    const S3Container& container, const S3Object& object)
{
    std::string ret = "{";
    ret += "\"key\" : \"" + object.m_key + "\", ";
    ret += "\"creation_time\" : \"" + object.m_creation_time + "\", ";
    ret += "\"content_length\" : \"" + object.m_content_length + "\", ";
    ret += "\"bucket\" : \"" + container.m_name + "\", ";

    auto on_item = [&ret](const std::string& key, const std::string& value) {
        ret += "\"" + key + "\" : \"" + value + "\", ";
    };
    object.m_user_metadata.for_each_item(on_item);

    ret = ret.substr(0, ret.size() - 2);
    ret += "}";
    return ret;
}

void S3ObjectModel::deserialize(
    S3Object& object, const Metadata& metadata, const std::string& prefix)
{
    auto on_item = [&object,
                    prefix](const std::string& key, const std::string& value) {
        const auto key_no_prefix = without_prefix(key, prefix);
        if (!is_internal_key(key_no_prefix)) {
            object.m_user_metadata.set_item(key_no_prefix, value);
        }
    };
    metadata.for_each_item(on_item);

    object.m_key            = metadata.get_item("key");
    object.m_creation_time  = metadata.get_item(prefix + ":creation_time");
    object.m_content_length = metadata.get_item(prefix + ":content_length");
    object.m_container      = metadata.get_item(prefix + ":bucket");
}

std::string S3ObjectModel::without_prefix(
    const std::string& key, const std::string& prefix)
{
    auto loc = key.find(prefix + ":");
    if (loc != key.npos) {
        return key.substr(prefix.size() + 1, key.size() - prefix.size() - 1);
    }
    else {
        return key;
    }
}

bool S3ObjectModel::is_internal_key(const std::string& key)
{
    return std::find(internal_keys.begin(), internal_keys.end(), key)
           != internal_keys.end();
}

std::string S3ObjectModel::get_query_filter(const std::string& prefix)
{
    return prefix + ":" + query_filter;
}
}  // namespace hestia