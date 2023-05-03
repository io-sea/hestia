#include "S3Client.h"

#include "LibS3InterfaceImpl.h"

extern "C" hestia::ObjectStoreClient* create()
{
    return new hestia::S3Client(std::make_unique<hestia::LibS3InterfaceImpl>());
}

extern "C" void destroy(hestia::ObjectStoreClient* object)
{
    delete object;
}