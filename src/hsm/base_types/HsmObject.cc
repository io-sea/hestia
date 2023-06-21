#include "HsmObject.h"

#include <algorithm>
#include <iostream>

namespace hestia {
HsmObject::HsmObject(const hestia::Uuid& id) : m_storage_object(id.to_string())
{
}

HsmObject::HsmObject(const hestia::StorageObject& object) :
    m_storage_object(object)
{
}

void HsmObject::add_tier(uint8_t tier_id, bool sort)
{
    if (auto it = std::find(m_tier_ids.begin(), m_tier_ids.end(), tier_id);
        it == m_tier_ids.end()) {
        m_tier_ids.push_back(tier_id);
    }
    if (sort) {
        std::sort(m_tier_ids.begin(), m_tier_ids.end());
    }
}

void HsmObject::remove_tier(uint8_t tier_id, bool sort)
{
    m_tier_ids.erase(
        std::remove(m_tier_ids.begin(), m_tier_ids.end(), tier_id),
        m_tier_ids.end());
    if (sort) {
        std::sort(m_tier_ids.begin(), m_tier_ids.end());
    }
}

void HsmObject::replace_tier(uint8_t old_id, uint8_t new_id)
{
    remove_tier(old_id, false);
    add_tier(new_id, false);
    std::sort(m_tier_ids.begin(), m_tier_ids.end());
}
}  // namespace hestia
