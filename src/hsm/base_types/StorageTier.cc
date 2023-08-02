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

StorageTier::StorageTier(uint8_t name) :
    HsmItem(HsmItem::Type::TIER), Model(HsmItem::tier_name)
{
    m_name.update_value(std::to_string(name));
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
        m_backends  = other.m_backends;
        m_capacity  = other.m_capacity;
        m_bandwidth = other.m_bandwidth;
        init();
    }
    return *this;
}

void StorageTier::init()
{
    m_name.set_index_on(true);

    register_foreign_key_proxy_field(&m_backends);
    register_scalar_field(&m_capacity);
    register_scalar_field(&m_bandwidth);
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

void StorageTier::set_capacity(std::size_t capacity)
{
    m_capacity.update_value(capacity);
}

void StorageTier::set_bandwidth(std::size_t bandwidth)
{
    m_bandwidth.update_value(bandwidth);
}

uint8_t StorageTier::id_uint() const
{
    if (m_name.get_value().empty()) {
        return 0;
    }
    return std::stoul(m_name.get_value());
}

const std::vector<ObjectStoreBackend>& StorageTier::get_backends() const
{
    return m_backends.models();
}

}  // namespace hestia
