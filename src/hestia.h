#include "types.h"
#include <vector>

namespace hestia {

void create_object(const struct hsm_uint& oid, struct hsm_obj& obj);

int put(
    const struct hsm_uint oid,
    struct hsm_obj* obj,
    const bool is_overwrite,
    const void* buf,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier = 0);

int get(
    const struct hsm_uint oid,
    struct hsm_obj* obj,
    void* buf,
    const std::size_t off,
    const std::size_t len,
    const std::uint8_t src_tier = 0,
    const std::uint8_t tgt_tier = 0);

int set_attrs(const struct hsm_uint& oid, const char* attrs);

std::string get_attrs(const struct hsm_uint& oid, char* attr_keys);

int remove(const struct hsm_uint& oid);

std::vector<struct hsm_uint> list(const std::uint8_t tier = 0);

}  // namespace hestia
