#include "../test/kv_store/disk.h"
#include "hestia.h"

std::vector<uint8_t> hestia::locate(const struct hsm_uint& oid)
{
    kv::Disk kv_store;

    // if (!kv_store.object_exists(oid)) {
    //     return std::numeric_limits<std::uint8_t>::max();
    // }

    auto tiers_json = kv_store.get_meta_data(oid, "tiers");

    auto num_tiers = hestia::list_tiers().size();
    std::vector<uint8_t> location_tiers{};

    for (auto i = 0; i < static_cast<int>(num_tiers); i++) {
        if (tiers_json["tiers"][i] == true) {
            location_tiers.push_back(static_cast<std::uint8_t>(i));
        }
    }
    return location_tiers;
}
