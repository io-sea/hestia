#include "MockPhobosClient.h"

extern "C" hestia::ObjectStoreClient* create()
{
    return new hestia::mock::MockPhobosClient();
}

extern "C" void destroy(hestia::ObjectStoreClient* object)
{
    delete object;
}