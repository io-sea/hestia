#pragma once

#include "S3Bucket.h"
#include "S3Error.h"
#include "S3Object.h"

namespace hestia {
struct S3Response {
    S3Response(std::unique_ptr<HttpResponse> http_response)
    {
        if (http_response->error()) {
            m_status = S3Error(*http_response);
        }
        m_http_response = std::move(http_response);
    }

    using Ptr = std::unique_ptr<S3Response>;
    HttpResponse::Ptr m_http_response;
    S3Error m_status;
};

struct S3ListBucketResponse {
    void deserialize(const std::string& response_body);

    using Ptr = std::unique_ptr<S3ListBucketResponse>;
    std::vector<S3Bucket::Ptr> m_buckets;
    S3Owner m_owner;
    S3Error m_error;
};

struct S3ListObjectsResponse {
    S3ListObjectsResponse(bool is_version_2 = true) :
        m_is_version2(is_version_2)
    {
    }

    void deserialize(const std::string& response_body);
    using Ptr = std::unique_ptr<S3ListObjectsResponse>;

    bool m_is_version2{true};
    S3Error m_error;
    bool m_is_truncated{false};
    std::string m_marker;
    std::string m_next_marker;
    std::vector<S3Object> m_contents;
    std::string m_name;
    std::string m_prefix;
    std::string m_delimiter;
    std::size_t m_max_keys;
    std::vector<std::string> m_common_prefixes;
    std::string m_encoding_type;

    std::size_t m_key_count{0};
    std::string m_continuation_token;
    std::string m_next_continuation_token;
    std::string m_start_after;
};
}  // namespace hestia