#include "S3Client.h"

#include "LibS3InterfaceImpl.h"
#include "Logger.h"

extern "C" hestia::ObjectStoreClient* create()
{
    return new hestia::S3Client(std::make_unique<hestia::LibS3InterfaceImpl>());
}

extern "C" void destroy(hestia::ObjectStoreClient* object)
{
    delete object;
}

extern "C" void set_logger_context(const hestia::LoggerContext& logger_context)
{
    hestia::Logger::get_instance().initialize(logger_context);
}