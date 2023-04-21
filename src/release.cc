#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"
// #include "tiers.h"

int hestia::release(const struct hsm_uint& oid, const std::uint8_t src_tier)
{
    obj::Disk object_store;
    kv::Disk kv_store;

    if (!kv_store.object_exists(oid)) {
        return -3;
    }
    nlohmann::json dataset_test = kv_store.get_meta_data(oid, "dataset");
    if (dataset_test["dataset"] == true) {
        auto members     = hestia::get_dataset_members(oid);
        int error_number = 0;
        for (auto& it : members) {
            if (release(it, src_tier) != 0) {
                error_number = 1;
            }
        }
        return error_number;
    }
    nlohmann::json tier_info = kv_store.get_meta_data(oid, "tiers");

    if (!tier_info["tiers"][src_tier]) {
        return -1;
    }
    object_store.remove(oid, src_tier);
    tier_info["tiers"][src_tier] = false;

    kv_store.put_meta_data(oid, tier_info);

    return 0;
}
