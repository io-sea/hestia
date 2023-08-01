#include "MotrConfig.h"
#include "MotrHsmClient.h"
#include "MotrInterface.h"

#include "Logger.h"

#ifdef HAS_MOTR  // Keep linter happy
#include "MotrInterfaceImpl.h"

extern "C" hestia::HsmObjectStoreClient* create()
{
    return new hestia::MotrHsmClient(std::make_unique<hestia::MotrInterface>(
        std::make_unique<hestia::MotrInterfaceImpl>()));
}

#endif

extern "C" void destroy(hestia::HsmObjectStoreClient* object)
{
    delete object;
}

extern "C" void set_logger_context(const hestia::LoggerContext& logger_context)
{
    hestia::Logger::get_instance().initialize(logger_context);
}
