#include "StorageTier.h"

namespace hestia {
StorageTier::StorageTier() : Model("tier") {}

StorageTier::StorageTier(const std::string& name) : Model(name, "tier") {}

StorageTier::StorageTier(uint8_t name) : Model(std::to_string(name), "tier") {}

uint8_t StorageTier::id_uint() const
{
    if (m_name.empty()) {
        return 0;
    }
    return std::stoul(m_name);
}

void StorageTier::deserialize(const Dictionary& dict, SerializeFormat format)
{
    Model::deserialize(dict, format);

    if (format == SerializeFormat::ID) {
        return;
    }

    Metadata scalar_data;
    dict.get_map_items(scalar_data);

    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "backend" && !value.empty()) {
            m_backend = value;
        }
        else if (key == "capacity" && !value.empty()) {
            m_capacity = std::stoul(value);
        }
        else if (key == "bandwidth" && !value.empty()) {
            m_bandwith = std::stoul(value);
        }
    };
    scalar_data.for_each_item(on_item);
}

void StorageTier::serialize(
    Dictionary& dict, SerializeFormat format, const Uuid& id) const
{
    Model::serialize(dict, format, id);

    if (format == SerializeFormat::ID) {
        return;
    }

    std::unordered_map<std::string, std::string> data{
        {"backend", m_backend},
        {"capacity", std::to_string(m_capacity)},
        {"bandwidth", std::to_string(m_bandwith)}};
    dict.set_map(data);
}

}  // namespace hestia
