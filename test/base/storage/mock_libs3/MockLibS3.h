#pragma once

#include "MockS3Server.h"

#include <cstdlib>
#include <functional>

namespace hestia::mock::libs3 {

enum class S3Status { OK, NOT_FOUND, UNKNOWN_ERROR };

struct S3ErrorDetails {
};

struct S3BucketContext {
};

struct S3GetConditions {
};

struct S3PutProperties {
};

struct S3RequestContext {
};

struct S3ResponseProperties {
};

using S3ResponseCompleteCallback   = std::function<void(
    S3Status status, const S3ErrorDetails* error_details, void* callback_data)>;
using S3ResponsePropertiesCallback = std::function<S3Status(
    const S3ResponseProperties* properties, void* callback_data)>;
struct S3ResponseHandler {
    S3ResponseProperties m_properties_callback;
    S3ResponseCompleteCallback m_response_complete_callback;
};

using S3PutObjectDataCallback =
    std::function<int(int buffer_size, char* buffer, void* callback_data)>;
struct S3PutObjectHandler {
    S3ResponseHandler m_response_handler;
    S3PutObjectDataCallback m_put_object_data_callback;
};

using S3GetObjectDataCallback = std::function<S3Status(
    int buffer_size, const char* buffer, void* callback_data)>;
struct S3GetObjectHandler {
    S3ResponseHandler m_response_handler;
    S3GetObjectDataCallback m_get_object_data_callback;
};

class LibS3 {
  public:
    S3Status s3_initialize(
        const char* user_agent_info,
        int flags,
        const char* default_s3_host_name);

    void s3_deinitialize();

    void s3_get_object(
        const S3BucketContext* bucket_context,
        const char* key,
        const S3GetConditions* get_conditions,
        uint64_t start_byte,
        uint64_t byte_count,
        S3RequestContext* request_context,
        int timeout_ms,
        const S3GetObjectHandler* handler,
        void* callback_data);

    void s3_put_object(
        const S3BucketContext* bucket_context,
        const char* key,
        uint64_t content_length,
        const S3PutProperties* put_properties,
        S3RequestContext* request_context,
        int timeout_ms,
        const S3PutObjectHandler* handler,
        void* callback_data);

    void s3_delete_object(
        const S3BucketContext* bucket_context,
        const char* key,
        S3RequestContext* request_context,
        int timeout_ms,
        const S3ResponseHandler* handler,
        void* callback_data);

  protected:
    Server* server() { return &m_server; }

  private:
    Server m_server;
};
}  // namespace hestia::mock::libs3
