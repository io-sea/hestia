#include "HsmObject.h"

#include "Logger.h"

#include <algorithm>
#include <iostream>

namespace hestia {
HsmObject::HsmObject() :
    HsmItem(HsmItem::Type::OBJECT), LockableModel(HsmItem::hsm_object_name)
{
    init();
}

HsmObject::HsmObject(const std::string& id) :
    HsmItem(HsmItem::Type::OBJECT), LockableModel(id, HsmItem::hsm_object_name)
{
    init();
}

HsmObject::HsmObject(const HsmObject& other) :
    HsmItem(other), LockableModel(other)
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
        LockableModel::operator=(other);
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
    register_one_to_one_proxy_field(&m_metadata);
    register_foreign_key_proxy_field(&m_tier_extents);
    register_foreign_key_field(&m_dataset);
}

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
}

}  // namespace hestia
