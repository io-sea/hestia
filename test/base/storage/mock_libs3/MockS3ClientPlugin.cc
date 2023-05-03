#include "S3Client.h"

#include "MockLibS3InterfaceImpl.h"

extern "C" hestia::ObjectStoreClient* create()
{
    return new hestia::S3Client(
        std::make_unique<hestia::mock::MockLibS3InterfaceImpl>());
}

extern "C" void destroy(hestia::ObjectStoreClient* object)
{
    delete object;
}