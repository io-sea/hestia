#include "HsmObject.h"

HsmObject::HsmObject(const ostk::Uuid& id)
    : mStorageObject(id.toString())
{

}

HsmObject::HsmObject(const ostk::StorageObject& object)
    : mStorageObject(object)
{

}

void HsmObject::addTier(uint8_t tierId)
{
    mTierIds.push_back(tierId);
}

void HsmObject::replaceTier(uint8_t oldId, uint8_t newId)
{
    mTierIds.push_back(newId);
}