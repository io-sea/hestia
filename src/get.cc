#include "hestia.h"
#include "kv_store/disk.h"
#include "object_store/disk.h"
#include <chrono>
#include <fstream>
#include <iostream>


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

    obj::Disk object_store;
    object_store.get(oid, buf, len);


    kv::Disk kvs;
    kvs.get_meta_data(*obj);


    return 0;
}
