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

struct hsm_obj {
    struct hsm_uint oid;
    nlohmann::json meta_data;

    hsm_obj() {}
    hsm_obj(std::uint64_t val) : oid(val) {}
    hsm_obj(const hsm_uint& in) : oid(in) {}
};

}  // namespace hestia
