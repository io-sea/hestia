#pragma once

#include "Metadata.h"
#include "OwnableModel.h"
#include "StorageObject.h"
#include "TierExtents.h"
#include "TimedLock.h"

#include <map>

namespace hestia {
class HsmObject : public OwnableModel {
  public:
    HsmObject();

    HsmObject(const Uuid& id);

    HsmObject(const StorageObject& object);

    void add_extent(uint8_t tier_id, const Extent& extent);

    const Uuid& dataset() const;

    void deserialize(
        const Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL) override;

    const Metadata& metadata() const;

    Metadata& metadata();

    StorageObject& object();

    const StorageObject& object() const;

    void remove_extent(uint8_t tier_id, const Extent& extent);

    void remove_tier(uint8_t tier_id);

    void remove_all_but_one_tiers();

    void read_lock();

    void write_lock();

    void read_unlock();

    void write_unlock();

    bool is_read_locked();

    bool is_write_locked();

    void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const override;

    void set_creation_time(std::time_t time) override;

    void set_id(const Uuid& id) override;

    const std::map<uint8_t, TierExtents>& tiers() const;

    std::string to_string() const;

    void set_size(std::size_t size) { m_storage_object.m_size = size; }

    std::size_t size() const { return m_storage_object.m_size; }

    void set_dataset_id(const Uuid& id) { m_dataset = id; }

    void update_modified_time(std::time_t time) override;

  private:
    TimedLock m_read_lock;
    TimedLock m_write_lock;
    StorageObject m_storage_object;
    std::map<uint8_t, TierExtents> m_tier_extents;
    Uuid m_dataset;
};
}  // namespace hestia
