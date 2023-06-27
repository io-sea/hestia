#pragma once

#include "Dictionary.h"
#include "Extent.h"
#include "TimedLock.h"

#include <ctime>
#include <map>

namespace hestia {

class TierExtents {
  public:
    TierExtents(uint8_t tier_id = 0, const Extent& extent = {});

    void add_extent(const Extent& extent);

    void remove_extent(const Extent& extent);

    void read_lock();

    void write_lock();

    void read_unlock();

    void write_unlock();

    bool is_read_locked();

    bool is_write_locked();

    Dictionary::Ptr serialize() const;

    void deserialize(const Dictionary& dict);

    uint8_t tier() const { return m_tier_id; }

  private:
    uint8_t m_tier_id{0};
    TimedLock m_read_lock;
    TimedLock m_write_lock;
    std::map<std::size_t, Extent> m_extents;
};
}  // namespace hestia