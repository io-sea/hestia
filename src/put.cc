#include "../test/data_placement_engine/eejit.h"
#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"
#include "tiers.h"
#include <chrono>

void hestia::create_object(const struct hsm_uint& oid, struct hsm_obj& obj)
{
    obj     = *(new hsm_obj);
    obj.oid = oid;

    auto time_since_epoch =
        std::chrono::system_clock::now().time_since_epoch().count();

    obj.meta_data["creation_time"] = time_since_epoch;
    obj.meta_data["last_modified"] = time_since_epoch;
}

int hestia::put(
    const struct hsm_uint oid,
    struct hsm_obj* obj,
    const bool is_overwrite,
    const void* buf,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier)
{
    if (!is_overwrite) {
        create_object(oid, *obj);
    }
    else {
        obj->meta_data["last_modified"] =
            std::chrono::system_clock::now().time_since_epoch().count();
    }

    dpe::Eejit dpe(tiers);
    const auto chosen_tier = dpe.choose_tier(length, target_tier);

    /* add object tier to metadata */
    obj->meta_data["tier"] = chosen_tier;

    /* interface with kv store */
    kv::Disk kv_store;

    /*
     * if the object already exists and we are not attempting to overwrite or if
     * the object doesn't exist and we are attempting to overwrite we should
     * stop here and return an error (TODO: appropriate error code)
     */
    if (kv_store.object_exists(oid) != is_overwrite) {
        return 1;
    }

    kv_store.put_meta_data(*obj);

    /* send data to backend */
    obj::Disk object_store;
    object_store.put(
        oid, static_cast<const char*>(buf) + offset, length, chosen_tier);

    return 0;
}
