#include "TierExtents.h"

namespace hestia {

TierExtents::TierExtents() :
    HsmItem(HsmItem::Type::EXTENT), LockableModel(HsmItem::tier_extents_name)
{
    init();
}

TierExtents::TierExtents(uint8_t tier_priority, const Extent& extent) :
    HsmItem(HsmItem::Type::EXTENT), LockableModel(HsmItem::tier_extents_name)
{
    m_extents.get_container_as_writeable()[extent.m_offset] = extent;
    m_tier_priority.init_value(tier_priority);

    init();
}

TierExtents::~TierExtents() {}

TierExtents::TierExtents(const TierExtents& other) :
    HsmItem(HsmItem::Type::EXTENT), LockableModel(other)
{
    *this = other;
}

const std::map<std::size_t, Extent>& TierExtents::get_extents() const
{
    return m_extents.container();
}

const std::string& TierExtents::get_tier_id() const
{
    return m_tier.get_id();
}

const std::string& TierExtents::get_object_id() const
{
    return m_object.get_id();
}

const std::string& TierExtents::get_backend_id() const
{
    return m_backend.get_id();
}

void TierExtents::set_object_id(const std::string& id)
{
    m_object.set_id(id);
}

void TierExtents::set_tier_id(const std::string& id)
{
    m_tier.set_id(id);
}

void TierExtents::set_backend_id(const std::string& id)
{
    m_backend.set_id(id);
}

void TierExtents::set_tier_priority(uint8_t name)
{
    m_tier_priority.update_value(name);
}

TierExtents& TierExtents::operator=(const TierExtents& other)
{
    if (this != &other) {
        LockableModel::operator=(other);
        m_tier_priority = other.m_tier_priority;
        m_extents       = other.m_extents;
        m_object        = other.m_object;
        m_tier          = other.m_tier;
        m_backend       = other.m_backend;
        init();
    }
    return *this;
}

std::string TierExtents::get_type()
{
    return HsmItem::tier_extents_name;
}

void TierExtents::init()
{
    register_scalar_field(&m_tier_priority);
    register_sequence_field(&m_extents);

    register_foreign_key_field(&m_object);
    register_foreign_key_field(&m_tier);
    register_foreign_key_field(&m_backend);
}

void TierExtents::add_extent(const Extent& extent)
{
    m_extents.get_container_as_writeable()[extent.m_offset] = extent;
}

void TierExtents::remove_extent(const Extent& extent)
{
    if (m_extents.container().size() == 1) {
        m_extents.get_container_as_writeable().erase(extent.m_offset);
        m_extents.get_container_as_writeable()[0] = Extent(0, 0);
    }
}

bool TierExtents::empty() const
{
    return m_extents.container().empty();
}

std::size_t TierExtents::get_size() const
{
    if (m_extents.container().empty()) {
        return 0;
    }
    return m_extents.container().rbegin()->second.get_end();
}

}  // namespace hestia