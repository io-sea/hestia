#pragma once

#include "S3Request.h"

namespace hestia {
class S3ListObjectsRequest {

  public:
    S3ListObjectsRequest(const S3UserContext& user_context);

    S3ListObjectsRequest(const HttpRequest& req);

    void build_query(Map& query) const;

    bool m_is_v2_type{true};
    S3Request m_s3_request;
    std::string m_prefix;
    std::string m_marker;
    std::string m_delimiter;
    std::string m_continuation_token;
    std::string m_encoding_type;
    std::string m_fetch_owner;
    std::string m_start_after;
    std::size_t m_max_keys{0};
};
}  // namespace hestia