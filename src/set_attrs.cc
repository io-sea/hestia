#include "../test/kv_store/disk.h"
#include "hestia.h"
#include <chrono>

int hestia::set_attrs(const struct hsm_uint oid, const char* attrs)
{
    auto attrs_json = nlohmann::json::parse(attrs);

    /* interface with kv store */
    kv::Disk kv_store;
    kv_store.put_meta_data(oid, attrs_json);

    return 0;
}
