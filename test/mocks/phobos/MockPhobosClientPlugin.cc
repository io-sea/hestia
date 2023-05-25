#include "MockPhobosClient.h"

#include "Logger.h"

extern "C" hestia::ObjectStoreClient* create()
{
    return new hestia::mock::MockPhobosClient();
}

extern "C" void destroy(hestia::ObjectStoreClient* object)
{
    delete object;
}

extern "C" void set_logger_context(const hestia::LoggerContext& logger_context)
{
    hestia::Logger::get_instance().initialize(logger_context);
}