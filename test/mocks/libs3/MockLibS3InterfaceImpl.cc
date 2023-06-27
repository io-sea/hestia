#include "MockLibS3InterfaceImpl.h"

#include <string>

#include <iostream>

namespace hestia::mock {

MockLibS3InterfaceImpl::Ptr MockLibS3InterfaceImpl::create()
{
    return std::make_unique<MockLibS3InterfaceImpl>();
}

void MockLibS3InterfaceImpl::initialize(const S3Config& config)
{
    int flags{0};
    m_lib_s3.s3_initialize(
        config.m_user_agent.c_str(), flags, config.m_default_host.c_str());
}

int MockLibS3InterfaceImpl::put(
    const S3Object& obj, const Extent& extent, Stream* stream)
{
    (void)extent;

    libs3::S3BucketContext bucket;

    libs3::S3Status response_status{libs3::S3Status::OK};

    auto on_response_complete = [&response_status](
                                    libs3::S3Status status,
                                    const libs3::S3ErrorDetails* error_details,
                                    void* callback_data) {
        response_status = status;
        (void)status;
        (void)error_details;
        (void)callback_data;
    };

    auto on_put_data = [](int buffer_size, char* buffer, void* callback_data) {
        auto stream = reinterpret_cast<Stream*>(callback_data);
        WriteableBufferView buffer_view(buffer, buffer_size);
        const auto io_result = stream->read(buffer_view);
        return io_result.m_num_transferred;
    };

    libs3::S3PutObjectHandler put_handler;
    put_handler.m_response_handler.m_response_complete_callback =
        on_response_complete;
    put_handler.m_put_object_data_callback = on_put_data;

    m_lib_s3.s3_put_object(
        &bucket, obj.m_name.c_str(), stream->get_source_size(), nullptr,
        nullptr, 0, &put_handler, reinterpret_cast<void*>(stream));
    return static_cast<int>(!(response_status == libs3::S3Status::OK));
}

int MockLibS3InterfaceImpl::get(
    const S3Object& obj, const Extent& extent, Stream* stream)
{
    (void)extent;

    libs3::S3BucketContext bucket;

    libs3::S3Status response_status;

    auto on_response_complete = [&response_status](
                                    libs3::S3Status status,
                                    const libs3::S3ErrorDetails* error_details,
                                    void* callback_data) {
        response_status = status;
        (void)status;
        (void)error_details;
        (void)callback_data;
    };

    auto on_get_data = [](int buffer_size, const char* buffer,
                          void* callback_data) {
        auto stream = reinterpret_cast<Stream*>(callback_data);
        ReadableBufferView buffer_view(buffer, buffer_size);
        const auto io_result = stream->write(buffer_view);
        return io_result.ok() ? libs3::S3Status::OK :
                                libs3::S3Status::UNKNOWN_ERROR;
    };

    libs3::S3GetObjectHandler get_handler;
    get_handler.m_response_handler.m_response_complete_callback =
        on_response_complete;
    get_handler.m_get_object_data_callback = on_get_data;

    m_lib_s3.s3_get_object(
        &bucket, obj.m_name.c_str(), nullptr, 0, stream->get_sink_size(),
        nullptr, 0, &get_handler, reinterpret_cast<void*>(stream));

    return static_cast<int>(!(response_status == libs3::S3Status::OK));
}

int MockLibS3InterfaceImpl::remove(const S3Object& obj)
{
    libs3::S3BucketContext bucket;

    libs3::S3Status response_status;

    auto on_response_complete = [&response_status](
                                    libs3::S3Status status,
                                    const libs3::S3ErrorDetails* error_details,
                                    void* callback_data) {
        response_status = status;
        (void)status;
        (void)error_details;
        (void)callback_data;
    };

    libs3::S3ResponseHandler response_handler;
    response_handler.m_response_complete_callback = on_response_complete;

    m_lib_s3.s3_delete_object(
        &bucket, obj.m_name.c_str(), nullptr, 0, &response_handler, nullptr);

    return static_cast<int>(!(response_status == libs3::S3Status::OK));
}
}  // namespace hestia::mock