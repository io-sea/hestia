#include "S3Client.h"

#include "Logger.h"
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

extern "C" void set_logger_context(const hestia::LoggerContext& logger_context)
{
    hestia::Logger::get_instance().initialize(logger_context);
}