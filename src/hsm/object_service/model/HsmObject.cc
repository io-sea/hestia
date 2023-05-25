#include "HsmObject.h"

namespace hestia {
HsmObject::HsmObject(const hestia::Uuid& id) : m_storage_object(id.to_string())
{
}

HsmObject::HsmObject(const hestia::StorageObject& object) :
    m_storage_object(object)
{
}

void HsmObject::add_tier(uint8_t tier_id)
{
    m_tier_ids.push_back(tier_id);
}

void HsmObject::replace_tier(uint8_t old_id, uint8_t new_id)
{
    (void)old_id;
    m_tier_ids.push_back(new_id);
}
}  // namespace hestia