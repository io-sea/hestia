#include "HsmObject.h"

#include "UuidUtils.h"

#include <algorithm>
#include <iostream>

namespace hestia {
HsmObject::HsmObject() : OwnableModel("hsm_object") {}

HsmObject::HsmObject(const Uuid& id) : OwnableModel(id), m_storage_object(id) {}

HsmObject::HsmObject(const StorageObject& object) :
    OwnableModel(object.id()), m_storage_object(object)
{
    if (m_id.is_unset() && !m_storage_object.name().empty()) {
        m_name = m_storage_object.name();
        m_type = "hsm_object";
    }

    m_creation_time      = object.get_creation_time();
    m_last_modified_time = object.get_last_modified_time();
}

void HsmObject::add_extent(uint8_t tier_id, const Extent& extent)
{
    if (auto it = m_tier_extents.find(tier_id); it != m_tier_extents.end()) {
        it->second.add_extent(extent);
    }
    else {
        m_tier_extents.emplace(tier_id, TierExtents(tier_id, extent));
    }
}

void HsmObject::remove_extent(uint8_t tier_id, const Extent& extent)
{
    if (auto it = m_tier_extents.find(tier_id); it != m_tier_extents.end()) {
        it->second.remove_extent(extent);
    }
}

void HsmObject::remove_tier(uint8_t tier_id)
{
    m_tier_extents.erase(tier_id);
}

void HsmObject::update_modified_time(std::time_t time)
{
    Model::update_modified_time(time);
    m_storage_object.update_modified_time(time);
}

void HsmObject::set_creation_time(std::time_t time)
{
    Model::set_creation_time(time);
    m_storage_object.set_creation_time(time);
}

void HsmObject::set_id(const Uuid& id)
{
    Model::set_id(id);
    m_storage_object.set_id(id);
}

Metadata& HsmObject::metadata()
{
    return m_storage_object.m_metadata;
}

void HsmObject::read_lock()
{
    m_read_lock.lock();
}

void HsmObject::write_lock()
{
    m_write_lock.lock();
}

void HsmObject::read_unlock()
{
    m_read_lock.unlock();
}

void HsmObject::write_unlock()
{
    m_write_lock.unlock();
}

bool HsmObject::is_read_locked()
{
    return m_read_lock.m_active;
}

bool HsmObject::is_write_locked()
{
    return m_write_lock.m_active;
}

void HsmObject::remove_all_but_one_tiers() {}

StorageObject& HsmObject::object()
{
    return m_storage_object;
}

const StorageObject& HsmObject::object() const
{
    return m_storage_object;
}

const Metadata& HsmObject::metadata() const
{
    return m_storage_object.m_metadata;
}

const Uuid& HsmObject::dataset() const
{
    return m_dataset;
}

const std::map<uint8_t, TierExtents>& HsmObject::tiers() const
{
    return m_tier_extents;
}

std::string HsmObject::to_string() const
{
    return m_storage_object.to_string();
}

void HsmObject::deserialize(const Dictionary& dict, SerializeFormat format)
{
    OwnableModel::deserialize(dict, format);

    if (format == SerializeFormat::ID) {
        return;
    }

    Metadata scalar_data;
    dict.get_map_items(scalar_data);

    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "size" && !value.empty()) {
            m_storage_object.m_size = std::stoul(value);
        }
        else if (key == "dataset_id" && !value.empty()) {
            m_dataset = UuidUtils::from_string(value);
        }
    };
    scalar_data.for_each_item(on_item);

    for (const auto& [key, dict_item] : dict.get_map()) {
        if (key == "read_lock") {
            m_read_lock.deserialize(*dict_item);
        }
        else if (key == "write_lock") {
            m_write_lock.deserialize(*dict_item);
        }
        else if (key == "user_metadata") {
            dict_item->get_map_items(m_storage_object.m_metadata);
        }
        else if (key == "tiers") {
            if (dict_item->get_type() == Dictionary::Type::SEQUENCE) {
                TierExtents tier_extents;
                for (const auto& tiers_dict : dict_item->get_sequence()) {
                    tier_extents.deserialize(*tiers_dict);
                    m_tier_extents[tier_extents.tier()] = tier_extents;
                }
            }
        }
    }
}

void HsmObject::serialize(
    Dictionary& dict, SerializeFormat format, const Uuid& id) const
{
    OwnableModel::serialize(dict, format, id);

    if (format == SerializeFormat::ID) {
        return;
    }

    auto user_md_dict = std::make_unique<Dictionary>();
    user_md_dict->set_map(m_storage_object.m_metadata.get_raw_data());
    dict.set_map_item("user_metadata", std::move(user_md_dict));

    std::unordered_map<std::string, std::string> data{
        {"size", std::to_string(m_storage_object.m_size)},
        {"dataset_id", UuidUtils::to_string(m_dataset)}};
    dict.set_map(data);

    auto tiers_dict = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& [key, value] : m_tier_extents) {
        tiers_dict->add_sequence_item(value.serialize());
    }
    dict.set_map_item("tiers", std::move(tiers_dict));

    dict.set_map_item("read_lock", m_read_lock.serialize());
    dict.set_map_item("write_lock", m_write_lock.serialize());
}

}  // namespace hestia
