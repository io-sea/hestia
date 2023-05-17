#include "MockMotrClient.h"

extern "C" hestia::HsmObjectStoreClient* create()
{
    return new hestia::mock::MockMotrHsmClient();
}

extern "C" void destroy(hestia::HsmObjectStoreClient* object)
{
    delete object;
}