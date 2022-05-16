#include "../test/kv_store/disk.h"
#include "hestia.h"

std::string hestia::get_attrs(const struct hsm_uint& oid, char* attr_keys)
{
    nlohmann::json attrs_json;

    kv::Disk kv_store;

    if (!kv_store.object_exists(oid)) {
        return std::string();
    }

    if (kv_store.get_meta_data(oid, attr_keys, attrs_json) < 0) {
        return std::string();
    }

    return attrs_json.dump();
}
