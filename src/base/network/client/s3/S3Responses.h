#pragma once

#include "S3Bucket.h"
#include "S3Object.h"
#include "S3Status.h"

namespace hestia {
struct S3Response {
    S3Response(HttpResponse::Ptr http_response);

    bool is_ok() const;

    using Ptr = std::unique_ptr<S3Response>;
    HttpResponse::Ptr m_http_response;
    S3Status m_status;
};

class S3ListBucketResponse {

  public:
    S3ListBucketResponse() = default;

    S3ListBucketResponse(const S3Response& response);
    using Ptr = std::unique_ptr<S3ListBucketResponse>;

    void deserialize(const std::string& response_body);

    std::size_t get_num_buckets() const;

    bool ok() const;

    std::string to_string() const;

    std::vector<S3Bucket> m_buckets;
    S3Owner m_owner;
    S3Status m_error;
};

struct S3ListObjectsResponse {
    S3ListObjectsResponse() = default;

    S3ListObjectsResponse(const S3Response& response, bool is_version_2 = true);
    using Ptr = std::unique_ptr<S3ListObjectsResponse>;

    void deserialize(const std::string& response_body);

    std::size_t get_num_objects() const { return m_contents.size(); }

    bool ok() const { return m_error.is_ok(); }

    std::string to_string() const;

    bool m_is_version2{true};
    S3Status m_error;
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