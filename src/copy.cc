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
    kv::Disk kv_store;

    if (!kv_store.object_exists(oid)) {
        return -3;
    }

    nlohmann::json dataset_test = kv_store.get_meta_data(oid, "dataset");

    if (dataset_test["dataset"] == true) {
        auto members     = hestia::get_dataset_members(oid);
        int error_number = 0;
        for (auto& it : members) {
            auto check = copy(it, src_tier, tgt_tier);
            if (check == -1) {
                error_number = 1;
            }
            else if (check != 0) {
                return -2;
            }
        }
        return error_number;
    }

    nlohmann::json tier_info = kv_store.get_meta_data(oid, "tiers");
    if (!tier_info["tiers"][src_tier]) {
        return -1;
    }

    object_store.copy(oid, src_tier, tgt_tier);

    tier_info["tiers"][tgt_tier] = true;

    kv_store.put_meta_data(oid, tier_info);

    return 0;
}
