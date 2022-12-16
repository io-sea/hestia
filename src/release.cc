#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"
//#include "tiers.h"

int hestia::release(const struct hsm_uint& oid, const std::uint8_t src_tier)
{
    obj::Disk object_store;
    object_store.remove(oid, src_tier);

    kv::Disk kv_store;
    nlohmann::json tier_info = kv_store.get_meta_data(oid, "tiers");

    tier_info["tiers"][src_tier] = false;

    kv_store.put_meta_data(oid, tier_info);

    return 0;
}
