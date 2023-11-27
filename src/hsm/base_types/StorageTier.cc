#include "StorageTier.h"

namespace hestia {

StorageTier::StorageTier(const std::string& id) :
    HsmItem(HsmItem::Type::TIER), Model(id, HsmItem::tier_name)
{
    init();
}

StorageTier::StorageTier() :
    HsmItem(HsmItem::Type::TIER), Model(HsmItem::tier_name)
{
    init();
}

StorageTier::StorageTier(const StorageTier& other) :
    HsmItem(HsmItem::Type::TIER), Model(other)
{
    *this = other;
}

StorageTier& StorageTier::operator=(const StorageTier& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_backends     = other.m_backends;
        m_tier_extents = other.m_tier_extents;
        m_priority     = other.m_priority;
        m_capacity     = other.m_capacity;
        m_bandwidth    = other.m_bandwidth;
        init();
    }
    return *this;
}

void StorageTier::init()
{
    m_priority.set_index_scope(BaseField::IndexScope::GLOBAL);

    register_foreign_key_proxy_field(&m_backends);
    register_foreign_key_proxy_field(&m_tier_extents);
    register_scalar_field(&m_capacity);
    register_scalar_field(&m_bandwidth);
    register_scalar_field(&m_priority);
}

std::string StorageTier::get_type()
{
    return HsmItem::tier_name;
}

std::size_t StorageTier::get_capacity() const
{
    return m_capacity.get_value();
}

std::size_t StorageTier::get_bandwidth() const
{
    return m_bandwidth.get_value();
}

const std::vector<TierExtents>& StorageTier::get_extents() const
{
    return m_tier_extents.models();
}

void StorageTier::set_capacity(std::size_t capacity)
{
    m_capacity.update_value(capacity);
}

void StorageTier::set_bandwidth(std::size_t bandwidth)
{
    m_bandwidth.update_value(bandwidth);
}

uint8_t StorageTier::get_priority() const
{
    return m_priority.get_value();
}

void StorageTier::set_priority(uint8_t priority)
{
    m_priority.update_value(priority);
}

const std::vector<ObjectStoreBackend>& StorageTier::get_backends() const
{
    return m_backends.models();
}

}  // namespace hestia
