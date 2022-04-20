#include "hestia.h"
#include "kv_store/disk.h"
#include "object_store/disk.h"
#include <chrono>
#include <iostream>

void hestia::create_object(const struct hsm_uint& oid, struct hsm_obj& obj)
{
    obj     = *(new hsm_obj);
    obj.oid = oid;

    auto time_since_epoch = std::to_string(
        std::chrono::system_clock::now().time_since_epoch().count());

    obj.meta_data.insert({"creation_time", time_since_epoch});
    obj.meta_data.insert({"last_modified", time_since_epoch});

    std::cout << "time = " << obj.meta_data["creation_time"] << std::endl;
}

int hestia::put(
    const struct hsm_uint oid,
    struct hsm_obj* obj,
    const bool is_overwrite,
    const void* buf,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t tgt_tier)
{
    if (!is_overwrite) {
        obj = new hsm_obj;
        create_object(oid, *obj);
    }

    /* interface with kv store */
    kv::Disk kv_store;
    kv_store.put_meta_data(*obj);

    /* send data to backend */
    obj::Disk object_store;
    object_store.put(oid, buf, length);

    return 0;
}
