#include "StorageObject.h"

#include <chrono>

namespace hestia {

StorageObject::StorageObject(const std::string& id) :
    OwnableModel(id, "storage_object")
{
}

StorageObject::StorageObject(const Uuid& id) : OwnableModel(id) {}

void StorageObject::deserialize(const Dictionary& dict, SerializeFormat format)
{
    OwnableModel::deserialize(dict, format);

    if (format == SerializeFormat::ID) {
        return;
    }

    auto user_md = dict.get_map_item("user_metadata");
    if (user_md != nullptr) {
        user_md->get_map_items(m_metadata);
    }

    auto on_items = [this](const std::string& key, const std::string& value) {
        if (key == "size" && !value.empty()) {
            m_size = std::stoul(value);
        }
    };
    dict.for_each_scalar(on_items);
}

void StorageObject::serialize(
    Dictionary& dict, SerializeFormat format, const Uuid& id) const
{
    OwnableModel::serialize(dict, format, id);

    if (format == SerializeFormat::ID) {
        return;
    }

    auto user_md_dict = std::unique_ptr<Dictionary>();
    user_md_dict->set_map(m_metadata.get_raw_data());
    dict.set_map_item("user_metadata", std::move(user_md_dict));

    std::unordered_map<std::string, std::string> data{
        {"size", std::to_string(m_size)}};
    dict.set_map(data);
}

bool StorageObject::empty() const
{
    return m_size == 0;
}

std::string StorageObject::to_string() const
{
    std::string ret;
    ret += "Id: " + m_id.to_string() + " | ";
    ret += "Name: " + m_name + " | ";
    ret += "Metadata: {";
    ret += m_metadata.to_string() + "}";
    return ret;
}

}  // namespace hestia