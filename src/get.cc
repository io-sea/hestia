#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"

int hestia::get(
    struct hsm_uint oid,
    struct hsm_obj* obj,
    void* buf,
    std::size_t off,
    std::size_t len,
    std::uint8_t src_tier,
    std::uint8_t tgt_tier)
{
    /* TODO temporary */
    src_tier = tgt_tier;

    kv::Disk kvs;

    /*
     * check if the object exists before attempting a potentially expensive call
     * to the object store! (TODO: appropriate error code)
     */
    if (!kvs.object_exists(oid)) {
        return 1;
    }

    kvs.get_meta_data(*obj);

    obj::Disk object_store;
    object_store.get(oid, static_cast<char*>(buf) + off, len, src_tier);

    return 0;
}
