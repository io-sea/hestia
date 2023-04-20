#pragma once

#include <ostk/StorageObject.h>
#include <ostk/Uuid.h>

#include <vector>

class HsmObject {
  public:
    HsmObject(const ostk::Uuid& id);

    HsmObject(const ostk::StorageObject& object);

    void addTier(uint8_t tierId);

    void removeTier(uint8_t tierId) {}

    void removeAllButOneTiers() {}

    void replaceTier(uint8_t oldId, uint8_t newId);

    ostk::StorageObject& object() { return mStorageObject; }

    const std::string& dataset() { return mDataset; }

    const std::vector<uint8_t>& tiers() const { return mTierIds; }

  private:
    ostk::Uuid mId;
    ostk::StorageObject mStorageObject;
    std::vector<uint8_t> mTierIds;
    std::string mDataset;
};