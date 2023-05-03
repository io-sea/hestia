#include "PhobosClient.h"

#include "PhobosInterface.h"

#ifdef HAS_PHOBOS  // Keep linter happy
#include "PhobosInterfaceImpl.h"

extern "C" hestia::ObjectStoreClient* create()
{
    return new hestia::PhobosClient(hestia::PhobosInterface::create(
        std::make_unique<hestia::PhobosInterfaceImpl>()));
}

extern "C" void destroy(hestia::ObjectStoreClient* object)
{
    delete object;
}
#endif