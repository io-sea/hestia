#pragma once

#include <cstddef>
#include <cstdint>

namespace hestia {

enum storage_class { ssd_nvme, ssd_sata, hdd, tape };

using tier_id_t = std::uint8_t;

class HsmTier {
  public:
    tier_id_t id = 0;

    enum storage_class store;
    std::size_t capacity;
    std::size_t bandwith;

    HsmTier() {}
    HsmTier(
        tier_id_t id,
        storage_class store,
        std::size_t capacity,
        std::size_t bandwith) :
        id(id), store(store), capacity(capacity), bandwith(bandwith)
    {
    }

    HsmTier(storage_class store, std::size_t capacity, std::size_t bandwith) :
        store(store), capacity(capacity), bandwith(bandwith)
    {
    }

    bool operator==(const HsmTier& other) const
    {
        return (other.id == id && other.store == store)
               && (other.capacity == capacity && other.bandwith == bandwith);
    }
};
}  // namespace hestia