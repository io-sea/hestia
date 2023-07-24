#include "HsmObject.h"

#include "Logger.h"

#include <algorithm>
#include <iostream>

namespace hestia {
HsmObject::HsmObject() :
    HsmItem(HsmItem::Type::OBJECT),
    LockableOwnableModel(HsmItem::hsm_object_name, {})
{
    init();
}

HsmObject::HsmObject(const std::string& id) :
    HsmItem(HsmItem::Type::OBJECT),
    LockableOwnableModel(id, HsmItem::hsm_object_name, {})
{
    init();
}

HsmObject::HsmObject(const HsmObject& other) :
    HsmItem(other), LockableOwnableModel(other)
{
    *this = other;
}

std::string HsmObject::get_type()
{
    return HsmItem::hsm_object_name;
}

HsmObject& HsmObject::operator=(const HsmObject& other)
{
    if (this != &other) {
        LockableOwnableModel::operator=(other);
        m_size         = other.m_size;
        m_metadata     = other.m_metadata;
        m_tier_extents = other.m_tier_extents;
        m_dataset      = other.m_dataset;
        init();
    }
    return *this;
}

void HsmObject::init()
{
    register_scalar_field(&m_size);
    register_map_field(&m_metadata);
    register_foreign_key_proxy_field(&m_tier_extents);
    register_named_foreign_key_field(&m_dataset);
}

/*
void HsmObject::add_extent(uint8_t tier_id, const Extent& extent)
{
    Extent working_extent = extent;
    if (working_extent.empty()) {
        working_extent = Extent(0, m_size.get_value());
    }

    const auto id_str = std::to_string(tier_id);

    if (auto it = m_tier_extents.get_container_as_writeable().find(id_str);
        it != m_tier_extents.get_container_as_writeable().end()) {
        it->second.add_extent(working_extent);
        const auto size = it->second.get_size();
        if (m_size.get_value() < size) {
            m_size.update_value(size);
        }
    }
    else {
        m_tier_extents.get_container_as_writeable().emplace(
            id_str, TierExtents(tier_id, working_extent));
        if (m_size.get_value() < working_extent.get_end()) {
            m_size.update_value(working_extent.get_end());
        }
    }
}
*/

/*
void HsmObject::remove_extent(uint8_t tier_id, const Extent& extent)
{
    Extent working_extent = extent;
    if (working_extent.empty()) {
        working_extent = Extent(0, m_size.get_value());
    }

    bool remove_tier{false};
    const auto id_str = std::to_string(tier_id);
    if (auto it = m_tier_extents.get_container_as_writeable().find(id_str);
        it != m_tier_extents.get_container_as_writeable().end()) {
        it->second.remove_extent(extent);
        std::size_t max_size{0};
        for (const auto& [key, tier] : m_tier_extents.container()) {
            const auto tier_size = tier.get_size();
            if (tier_size > max_size) {
                max_size = tier_size;
            }
        }
        m_size.update_value(max_size);
        if (it->second.empty()) {
            remove_tier = true;
        }
    }
    if (remove_tier) {
        m_tier_extents.get_container_as_writeable().erase(id_str);
    }
}
*/

/*
void HsmObject::remove_tier(uint8_t tier_id)
{
    m_tier_extents.get_container_as_writeable().erase(std::to_string(tier_id));
}*/

/*
void HsmObject::remove_all_tiers()
{
    m_tier_extents.get_container_as_writeable().clear();
}
*/

const Map& HsmObject::metadata() const
{
    return m_metadata.value().data();
}

const std::string& HsmObject::dataset() const
{
    return m_dataset.get_id();
}

const std::vector<TierExtents>& HsmObject::tiers() const
{
    return m_tier_extents.models();
}

void HsmObject::set_size(std::size_t size)
{
    m_size.update_value(size);
}

std::size_t HsmObject::size() const
{
    return m_size.get_value();
}

void HsmObject::set_dataset_id(const std::string& id)
{
    m_dataset.set_id(id);
}

std::string HsmObject::to_string() const
{
    return "";
    // return m_storage_object.to_string();
}

}  // namespace hestia
