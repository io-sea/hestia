#include "S3ContainerModel.h"

namespace hestia {
const std::vector<std::string> S3ContainerModel::internal_keys{
    "key", "creation_time", "is-bucket", "protocol"};

Metadata S3ContainerModel::serialize(
    const S3Container& container, const std::string& prefix)
{
    Metadata metadata;
    metadata.set_item("key", container.m_name);
    metadata.set_item(prefix + ":creation_time", container.m_creation_time);
    metadata.set_item(prefix + ":is-bucket", "yes");
    metadata.set_item(prefix + ":protocol", "s3");

    metadata.merge(container.m_user_metadata);

    return metadata;
}

std::string S3ContainerModel::json_serialize(
    const hestia::S3Container& container)
{
    std::string ret = "{";
    ret += "\"key\" : \"" + container.m_name + "\", ";
    ret += "\"creation_time\" : \"" + container.m_creation_time + "\", ";

    auto on_item = [&ret](const std::string& key, const std::string& value) {
        ret += "\"" + key + "\" : \"" + value + "\", ";
    };
    container.m_user_metadata.for_each_item(on_item);

    ret = ret.substr(0, ret.size() - 2);
    ret += "}";
    return ret;
}

void S3ContainerModel::deserialize(
    S3Container& container, const Metadata& metadata, const std::string& prefix)
{
    container.m_name          = metadata.get_item("key");
    container.m_creation_time = metadata.get_item(prefix + ":creation_time");

    auto on_item = [&container,
                    prefix](const std::string& key, const std::string& value) {
        const auto key_no_prefix = without_prefix(key, prefix);
        if (!is_internal_key(key_no_prefix)) {
            container.m_user_metadata.set_item(key_no_prefix, value);
        }
    };
    metadata.for_each_item(on_item);
}

std::string S3ContainerModel::without_prefix(
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

bool S3ContainerModel::is_internal_key(const std::string& key)
{
    return std::find(internal_keys.begin(), internal_keys.end(), key)
           != internal_keys.end();
}

Metadata::Query S3ContainerModel::get_query_filter(const std::string& prefix)
{
    return {prefix + ":is-bucket", "yes"};
}
}  // namespace hestia