#pragma once

#include <cstddef>
#include <cstdint>

#include <nlohmann/json.hpp>

namespace hestia {

struct hsm_uint {
    std::uint64_t higher = 0;
    std::uint64_t lower  = 0;

    hsm_uint() {}
    hsm_uint(std::uint64_t val) : lower(val) {}
    hsm_uint(std::uint64_t higher, std::uint64_t lower) :
        higher(higher), lower(lower)
    {
    }

    bool operator==(const hsm_uint& other) const
    {
        return other.higher == higher && other.lower == lower;
    }
};

enum storage_class { ssd_nvme, ssd_sata, hdd, tape };

struct hsm_tier {
    std::uint8_t id = 0;

    enum storage_class store;
    std::size_t capacity;
    std::size_t bandwith;

    hsm_tier() {}
    hsm_tier(
        std::uint8_t id,
        storage_class store,
        std::size_t capacity,
        std::size_t bandwith) :
        id(id), store(store), capacity(capacity), bandwith(bandwith)
    {
    }
    hsm_tier(storage_class store, std::size_t capacity, std::size_t bandwith) :
        store(store), capacity(capacity), bandwith(bandwith)
    {
    }

    bool operator==(const hsm_tier& other) const
    {
        return (other.id == id && other.store == store)
               && (other.capacity == capacity && other.bandwith == bandwith);
    }
};

}  // namespace hestia
