#include "MotrConfig.h"
#include "MotrHsmClient.h"
#include "MotrInterface.h"

#ifdef HAS_MOTR  // Keep linter happy
#include "MotrInterfaceImpl.h"

extern "C" hestia::HsmObjectStoreClient* create()
{
    return new hestia::MotrHsmClient();
}

extern "C" void destroy(hestia::HsmObjectStoreClient* object)
{
    delete object;
}
#endif
