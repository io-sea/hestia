#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"

int hestia::remove(const struct hsm_uint oid)
{
    /* interface with kv store to erase metadata */
    kv::Disk kv_store;

    /*
     * get the tier of storage the object resides in
     * TODO: this can be replaced with a call to get_attrs once it is
     * implemented
     */
    struct hsm_obj obj(oid);
    kv_store.get_meta_data(obj);

    const std::uint8_t tier = obj.meta_data["tier"];

    kv_store.remove(oid);

    /* request for object to be removed from backend storage */
    obj::Disk object_store;
    object_store.remove(oid, tier);

    return 0;
}
