#include "HsmObject.h"

HsmObject::HsmObject(const ostk::Uuid& id) : mStorageObject(id.toString()) {}

HsmObject::HsmObject(const ostk::StorageObject& object) : mStorageObject(object)
{
}

void HsmObject::add_tier(uint8_t tier_id)
{
    m_tier_ids.push_back(tierId);
}

void HsmObject::replace_tier(uint8_t old_id, uint8_t new_id)
{
    m_tier_ids.push_back(newId);
}