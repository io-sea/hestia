#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"
#include <chrono>
#include <fstream>

int hestia::get(
    struct hsm_uint oid,
    struct hsm_obj* obj,
    void* buf,
    std::size_t off,
    std::size_t len,
    std::uint8_t src_tier,
    std::uint8_t tgt_tier)
{
    // check for existance when that check is implemented

    /* TODO temporary */
    src_tier = tgt_tier;

    obj::Disk object_store;
    object_store.get(oid, static_cast<char*>(buf) + off, len, src_tier);

    kv::Disk kvs;
    kvs.get_meta_data(*obj);

    return 0;
}
