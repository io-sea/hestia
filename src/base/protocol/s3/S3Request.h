#pragma once

#include "Map.h"
#include "S3Path.h"
#include "S3Types.h"
#include <string>

namespace hestia {

enum class S3UriStyle { VIRTUAL_HOST, PATH };

struct S3UserContext {
    std::string m_user_id;
    std::string m_user_secret_key;
};

struct S3Request {
    S3Request() = default;
    S3Request(const HttpRequest& req)
    {
        m_path        = S3Path(req);
        m_tracking_id = req.get_tracking_id();
    }

    S3Path m_path;
    S3UserContext m_user_context;
    S3UriStyle m_uri_style{S3UriStyle::PATH};
    std::string m_endpoint;
    std::string m_region;
    S3Range m_range;
    std::string m_tracking_id;
};

struct S3ListObjectRequest {
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