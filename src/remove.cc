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
    struct hsm_obj obj(oid);
    std::string tier_key = "tier";

    // const auto tier = kv_store.get_meta_data(obj.oid, tier_key)[tier_key];
    const auto tiers = hestia::locate(oid);
    kv_store.remove(oid);

    /* request for object to be removed from backend storage */
    obj::Disk object_store;
    for (const auto& it : tiers) {
        object_store.remove(oid, it);
    }

    return 0;
}
