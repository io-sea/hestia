#include "TierExtents.h"

namespace hestia {

TierExtents::TierExtents() :
    HsmItem(HsmItem::Type::EXTENT), LockableModel(HsmItem::tier_extents_name)
{
    init();
}

TierExtents::TierExtents(uint8_t tier_id, const Extent& extent) :
    HsmItem(HsmItem::Type::EXTENT), LockableModel(HsmItem::tier_extents_name)
{
    m_extents.get_container_as_writeable()[extent.m_offset] = extent;
    m_tier_id.init_value(tier_id);

    init();
}

TierExtents::~TierExtents() {}

TierExtents::TierExtents(const TierExtents& other) :
    HsmItem(HsmItem::Type::EXTENT), LockableModel(other)
{
    *this = other;
}

TierExtents& TierExtents::operator=(const TierExtents& other)
{
    if (this != &other) {
        LockableModel::operator=(other);
        m_tier_id = other.m_tier_id;
        m_extents = other.m_extents;
        m_object  = other.m_object;
        m_tier    = other.m_tier;
        m_backend = other.m_backend;
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
    register_scalar_field(&m_tier_id);
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