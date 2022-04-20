#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace hestia {

struct hsm_uint {
    std::uint64_t higher = 0;
    std::uint64_t lower  = 0;

    hsm_uint() {}
    hsm_uint(std::uint64_t val) : lower(val) {}
    hsm_uint(const hsm_uint& in) : higher(in.higher), lower(in.lower) {}
};

struct hsm_obj {
    struct hsm_uint oid;
    std::unordered_map<std::string, std::string> meta_data;

    hsm_obj() {}
    hsm_obj(std::uint64_t val) : oid(val) {}
    hsm_obj(const hsm_uint& in) : oid(in) {}
};

}  // namespace hestia
