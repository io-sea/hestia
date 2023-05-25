#include "MockLibS3.h"

namespace hestia::mock::libs3 {

S3Status LibS3::s3_initialize(
    const char* user_agent_info, int flags, const char* default_s3_host_name)
{
    (void)user_agent_info;
    (void)flags;
    (void)default_s3_host_name;

    return {};
}

void LibS3::s3_deinitialize() {}

void LibS3::s3_get_object(
    const S3BucketContext* bucket_context,
    const char* key,
    const S3GetConditions* get_conditions,
    uint64_t start_byte,
    uint64_t byte_count,
    S3RequestContext* request_context,
    int timeout_ms,
    const S3GetObjectHandler* handler,
    void* callback_data)
{
    (void)bucket_context;
    (void)get_conditions;
    (void)start_byte;
    (void)byte_count;
    (void)timeout_ms;
    (void)request_context;

    auto response = m_server.get(key);

    S3Status status = S3Status::OK;
    if (response.m_status != Server::S3Status::OK) {
        switch (response.m_status) {
            case Server::S3Status::NOT_FOUND:
                status = S3Status::NOT_FOUND;
                break;
            case Server::S3Status::OK:
                break;
            default:
                status = S3Status::UNKNOWN_ERROR;
        }

        S3ErrorDetails error_details;
        handler->m_response_handler.m_response_complete_callback(
            status, &error_details, callback_data);
    }
    else {
        handler->m_get_object_data_callback(
            response.m_body.size(), response.m_body.data(), callback_data);

        S3ErrorDetails error_details;
        handler->m_response_handler.m_response_complete_callback(
            status, &error_details, callback_data);
    }
}

void LibS3::s3_put_object(
    const S3BucketContext* bucket_context,
    const char* key,
    uint64_t content_length,
    const S3PutProperties* put_properties,
    S3RequestContext* request_context,
    int timeout_ms,
    const S3PutObjectHandler* handler,
    void* callback_data)
{
    (void)bucket_context;
    (void)timeout_ms;
    (void)request_context;
    (void)content_length;
    (void)put_properties;

    std::vector<char> buffer;

    std::size_t block_size(100);
    std::vector<char> block(block_size);

    int num_bytes = 1;
    while (num_bytes > 0) {
        num_bytes = handler->m_put_object_data_callback(
            block_size, block.data(), callback_data);

        buffer.insert(buffer.end(), block.begin(), block.begin() + num_bytes);

        block.clear();
        block.resize(block_size);
    }

    auto response = m_server.put(key, buffer);

    S3Status status = S3Status::OK;
    S3ErrorDetails error_details;
    handler->m_response_handler.m_response_complete_callback(
        status, &error_details, callback_data);
}

void LibS3::s3_delete_object(
    const S3BucketContext* bucket_context,
    const char* key,
    S3RequestContext* request_context,
    int timeout_ms,
    const S3ResponseHandler* handler,
    void* callback_data)
{
    (void)bucket_context;
    (void)timeout_ms;
    (void)request_context;

    auto response = m_server.remove(key);

    S3Status status = S3Status::OK;
    if (response.m_status != Server::S3Status::OK) {
        switch (response.m_status) {
            case Server::S3Status::NOT_FOUND:
                status = S3Status::NOT_FOUND;
                break;
            case Server::S3Status::OK:
                break;
            default:
                status = S3Status::UNKNOWN_ERROR;
        }
        S3ErrorDetails error_details;
        handler->m_response_complete_callback(
            status, &error_details, callback_data);
    }
    else {

        S3ErrorDetails error_details;
        handler->m_response_complete_callback(
            status, &error_details, callback_data);
    }
}

}  // namespace hestia::mock::libs3