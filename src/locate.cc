#include "../test/kv_store/disk.h"
#include "hestia.h"

std::uint8_t hestia::locate(const struct hsm_uint& oid)
{
    kv::Disk kv_store;

    if (!kv_store.object_exists(oid)) {
        return std::numeric_limits<std::uint8_t>::max();
    }

    auto tier_json = kv_store.get_meta_data(oid, "tier");
    return tier_json["tier"];
}
