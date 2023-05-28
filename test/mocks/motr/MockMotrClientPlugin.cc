#include "MockMotrClient.h"

#include "Logger.h"

extern "C" hestia::HsmObjectStoreClient* create()
{
    return new hestia::mock::MockMotrHsmClient();
}

extern "C" void destroy(hestia::HsmObjectStoreClient* object)
{
    delete object;
}

extern "C" void set_logger_context(const hestia::LoggerContext& logger_context)
{
    hestia::Logger::get_instance().initialize(logger_context);
}