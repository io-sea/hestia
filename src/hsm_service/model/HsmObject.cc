#include "HsmObject.h"

HsmObject::HsmObject(const ostk::Uuid& id) : m_storage_object(id.toString()) {}

HsmObject::HsmObject(const ostk::StorageObject& object) :
    m_storage_object(object)
{
}

void HsmObject::add_tier(uint8_t tier_id)
{
    m_tier_ids.push_back(tier_id);
}

void HsmObject::replace_tier(uint8_t old_id, uint8_t new_id)
{
    m_tier_ids.push_back(new_id);
}