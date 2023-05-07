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
    (void)config;
    std::string user_agent;
    std::string default_host;
    int flags{0};
    m_lib_s3.s3_initialize(user_agent.c_str(), flags, default_host.c_str());
}

int MockLibS3InterfaceImpl::put(
    const S3Object& obj,
    const Extent& extent,
    const ReadableBufferView* read_buffer)
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

    std::size_t buffer_loc{0};
    auto on_put_data = [&buffer_loc, read_buffer](
                           int buffer_size, char* buffer, void* callback_data) {
        (void)callback_data;
        int bytes_written = 0;
        for (int idx = 0; idx < buffer_size; idx++) {
            if (buffer_loc == read_buffer->length()) {
                break;
            }
            buffer[idx] = *(read_buffer->data() + buffer_loc);
            buffer_loc++;
            bytes_written++;
        }
        return bytes_written;
    };

    libs3::S3PutObjectHandler put_handler;
    put_handler.m_response_handler.m_response_complete_callback =
        on_response_complete;
    put_handler.m_put_object_data_callback = on_put_data;

    m_lib_s3.s3_put_object(
        &bucket, obj.m_key.c_str(), read_buffer->length(), nullptr, nullptr, 0,
        &put_handler, nullptr);
    return static_cast<int>(!(response_status == libs3::S3Status::OK));
}

int MockLibS3InterfaceImpl::get(
    const S3Object& obj,
    const Extent& extent,
    WriteableBufferView* write_buffer)
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

    std::size_t buffer_loc{0};
    auto on_get_data = [write_buffer, &buffer_loc](
                           int buffer_size, const char* buffer,
                           void* callback_data) {
        (void)callback_data;
        for (int idx = 0; idx < buffer_size; idx++) {
            *(write_buffer->data() + buffer_loc) = buffer[idx];
            buffer_loc++;
        }
        return libs3::S3Status::OK;
    };

    libs3::S3GetObjectHandler get_handler;
    get_handler.m_response_handler.m_response_complete_callback =
        on_response_complete;
    get_handler.m_get_object_data_callback = on_get_data;

    m_lib_s3.s3_get_object(
        &bucket, obj.m_key.c_str(), nullptr, 0, write_buffer->length(), nullptr,
        0, &get_handler, nullptr);

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
        &bucket, obj.m_key.c_str(), nullptr, 0, &response_handler, nullptr);

    return static_cast<int>(!(response_status == libs3::S3Status::OK));
}
}  // namespace hestia::mock