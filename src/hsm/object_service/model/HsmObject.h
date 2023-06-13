#pragma once

#include "StorageObject.h"
#include "Uuid.h"

#include <vector>

namespace hestia {
class HsmObject {
  public:
    HsmObject() = default;

    HsmObject(const hestia::Uuid& id);

    HsmObject(const hestia::StorageObject& object);

    const hestia::Uuid& uuid() const { return m_id; }

    //HsmObject(const std::string& id) : m_storage_object.id()(id) {}

    const std::string& id() const { return m_storage_object.id(); }

    void add_tier(uint8_t tier_id, bool sort = true);

    void remove_tier(uint8_t tier_id, bool sort = true);

    void remove_all_but_one_tiers() {}

    void replace_tier(uint8_t old_id, uint8_t new_id);

    hestia::StorageObject& object() { return m_storage_object; }

    const hestia::StorageObject& object() const { return m_storage_object; }

    const std::string& dataset() { return m_dataset; }

    const std::vector<uint8_t>& tiers() const { return m_tier_ids; }

    std::string to_string() const { return m_storage_object.to_string(); }

  private:
    hestia::Uuid m_id;
    hestia::StorageObject m_storage_object;
    std::vector<uint8_t> m_tier_ids;
    std::string m_dataset;
};
}  // namespace hestia
