#include "hestia.h"

int main()
{
    struct hestia::hsm_uint oid(23905702934);
    struct hestia::hsm_obj obj;

    void* data = 0;

    return hestia::put(oid, &obj, false, data, 0, 0, 0);
}
