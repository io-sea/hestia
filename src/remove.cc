#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"

int hestia::remove(const struct hsm_uint& oid)
{
    /* interface with kv store to erase metadata */
    kv::Disk kv_store;

    /*
     * get the tier of storage the object resides in
     * TODO (suggestion): alternatively, kv_store.remove could return the tier
     * ID on success and a negative error code on error
     */
    //    std::string tier_key = "tier";


    //        int error_no = 0;
    nlohmann::json dataset_test = kv_store.get_meta_data(oid, "dataset");
    if (dataset_test["dataset"] == true) {
        auto members = hestia::get_dataset_members(oid);
        for (auto& it : members) {
            remove(it);
/*            if (remove(it) != 0) {
                error_no=1;
            }
*/        }
//        return 0;
    }


    const auto tiers = hestia::locate(oid);
    kv_store.remove(oid);

    /* request for object to be removed from backend storage */
    obj::Disk object_store;
    for (const auto& it : tiers) {
        object_store.remove(oid, it);
    }

    return 0;
}
