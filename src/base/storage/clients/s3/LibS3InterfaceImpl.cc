#include "LibS3InterfaceImpl.h"

#include "Logger.h"

#include <libs3.h>
#include <stdexcept>

namespace hestia {

LibS3InterfaceImpl::~LibS3InterfaceImpl()
{
    S3_deinitialize();
}

void LibS3InterfaceImpl::initialize(const S3Config& config)
{
    auto status = S3_initialize(
        "config.m_user_agent.c_str()", S3_INIT_ALL,
        config.m_default_host.get_value().c_str());
    if (status != S3Status::S3StatusOK) {
        throw std::runtime_error("Failed to initialize libs3");
    }
}

struct CallbackContext {
    S3Status m_response_status;
    Stream* m_stream{nullptr};
};

int LibS3InterfaceImpl::put(
    const S3Object& obj, const Extent& extent, Stream* stream)
{
    (void)extent;

    std::string bucket_name = "default";
    if (!obj.m_container.empty()) {
        bucket_name = obj.m_container;
    }

    S3BucketContext bucket = {
        0,      bucket_name.c_str(), S3ProtocolHTTP, S3UriStylePath, "", "", "",
        nullptr};

    CallbackContext cb_context;
    cb_context.m_response_status = S3Status::S3StatusOK;
    cb_context.m_stream          = stream;

    auto on_response_properties = [](const S3ResponseProperties* properties,
                                     void* callback_data) {
        (void)callback_data;
        LOG_INFO("Got response with length: " << properties->contentLength);
        return S3Status::S3StatusOK;
    };

    auto on_response_complete = [](S3Status status,
                                   const S3ErrorDetails* error_details,
                                   void* callback_data) {
        auto callback_context =
            reinterpret_cast<CallbackContext*>(callback_data);
        callback_context->m_response_status = status;
        (void)error_details;
    };

    auto on_put_data = [](int buffer_size, char* buffer,
                          void* callback_data) -> int {
        auto cb_context = reinterpret_cast<CallbackContext*>(callback_data);
        WriteableBufferView buffer_view(buffer, buffer_size);
        const auto io_result = cb_context->m_stream->read(buffer_view);
        return io_result.m_num_transferred;
    };

    S3PutObjectHandler put_handler;
    put_handler.responseHandler.propertiesCallback = on_response_properties;
    put_handler.responseHandler.completeCallback   = on_response_complete;
    put_handler.putObjectDataCallback              = on_put_data;

    LOG_INFO("Starting LibS3 put");
    S3_put_object(
        &bucket, obj.m_name.c_str(), stream->get_source_size(), nullptr,
        nullptr, 0, &put_handler, reinterpret_cast<void*>(&cb_context));

    LOG_INFO(
        "Finished LibS3 put - status is: "
        + std::to_string(static_cast<int>(cb_context.m_response_status)));

    return static_cast<int>(
        !(cb_context.m_response_status == S3Status::S3StatusOK));
}

int LibS3InterfaceImpl::get(
    const S3Object& obj, const Extent& extent, Stream* stream)
{
    (void)extent;

    S3BucketContext bucket;

    CallbackContext cb_context;
    cb_context.m_response_status = S3Status::S3StatusOK;
    cb_context.m_stream          = stream;

    auto on_response_complete = [](S3Status status,
                                   const S3ErrorDetails* error_details,
                                   void* callback_data) {
        auto callback_context =
            reinterpret_cast<CallbackContext*>(callback_data);
        callback_context->m_response_status = status;
        (void)error_details;
    };

    auto on_get_data = [](int buffer_size, const char* buffer,
                          void* callback_data) {
        auto cb_context = reinterpret_cast<CallbackContext*>(callback_data);
        ReadableBufferView buffer_view(buffer, buffer_size);
        const auto io_result = cb_context->m_stream->write(buffer_view);
        return io_result.ok() ? S3Status::S3StatusOK :
                                S3Status::S3StatusErrorUnknown;
    };

    S3GetObjectHandler get_handler;
    get_handler.responseHandler.completeCallback = on_response_complete;
    get_handler.getObjectDataCallback            = on_get_data;

    S3_get_object(
        &bucket, obj.m_name.c_str(), nullptr, 0, stream->get_sink_size(),
        nullptr, 0, &get_handler, reinterpret_cast<void*>(&cb_context));

    return static_cast<int>(
        !(cb_context.m_response_status == S3Status::S3StatusOK));
}

int LibS3InterfaceImpl::remove(const S3Object& obj)
{
    S3BucketContext bucket;

    CallbackContext cb_context;
    cb_context.m_response_status = S3Status::S3StatusOK;

    auto on_response_complete = [](S3Status status,
                                   const S3ErrorDetails* error_details,
                                   void* callback_data) {
        auto callback_context =
            reinterpret_cast<CallbackContext*>(callback_data);
        callback_context->m_response_status = status;
        (void)error_details;
    };

    S3ResponseHandler response_handler;
    response_handler.completeCallback = on_response_complete;

    S3_delete_object(
        &bucket, obj.m_name.c_str(), nullptr, 0, &response_handler,
        reinterpret_cast<void*>(&cb_context));

    return static_cast<int>(
        !(cb_context.m_response_status == S3Status::S3StatusOK));
}
}  // namespace hestia
