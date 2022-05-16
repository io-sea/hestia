#include "../test/kv_store/disk.h"
#include "hestia.h"

std::string hestia::list_attrs(const struct hsm_uint& oid)
{
    kv::Disk kv_store;

    if (!kv_store.object_exists(oid)) {
        return "";
    }

    return kv_store.get_meta_data(oid);
}
