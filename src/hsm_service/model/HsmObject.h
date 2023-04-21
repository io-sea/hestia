#pragma once

#include <ostk/StorageObject.h>
#include <ostk/Uuid.h>

#include <vector>

class HsmObject {
  public:
    HsmObject(const ostk::Uuid& id);

    HsmObject(const ostk::StorageObject& object);

    void add_tier(uint8_t tier_id);

    void remove_tier(uint8_t tier_id) {}

    void remove_all_but_one_tiers() {}

    void replace_tier(uint8_t old_id, uint8_t new_id);

    ostk::StorageObject& object() { return mStorageObject; }

    const std::string& dataset() { return mDataset; }

    const std::vector<uint8_t>& tiers() const { return mTierIds; }

  private:
    ostk::Uuid m_id;
    ostk::StorageObject m_storage_object;
    std::vector<uint8_t> m_tier_ids;
    std::string m_dataset;
};