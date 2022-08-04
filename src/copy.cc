#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"
//#include "tiers.h"

int hestia::copy(
    const struct hsm_uint& oid,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier)
{
    obj::Disk object_store;
    object_store.copy(oid, src_tier, tgt_tier);

    kv::Disk kv_store;
    nlohmann::json tier_info;

    tier_info["copy"] = tgt_tier;

    kv_store.put_meta_data(oid, tier_info);

    return 0;
}
