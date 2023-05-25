#include "PhobosClient.h"

#include "Logger.h"
#include "PhobosInterface.h"

#ifdef HAS_PHOBOS  // Keep linter happy
#include "PhobosInterfaceImpl.h"

extern "C" hestia::ObjectStoreClient* create()
{
    return new hestia::PhobosClient(hestia::PhobosInterface::create(
        std::make_unique<hestia::PhobosInterfaceImpl>()));
}

#endif

extern "C" void destroy(hestia::ObjectStoreClient* object)
{
    delete object;
}

extern "C" void set_logger_context(const hestia::LoggerContext& logger_context)
{
    hestia::Logger::get_instance().initialize(logger_context);
}