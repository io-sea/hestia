#include "S3ListObjectsRequest.h"

namespace hestia {

S3ListObjectsRequest::S3ListObjectsRequest(const S3UserContext& user_context) :
    m_s3_request(user_context)
{
}

S3ListObjectsRequest::S3ListObjectsRequest(const HttpRequest& req) :
    m_s3_request(req)
{
}

void S3ListObjectsRequest::build_query(Map& query) const
{
    if (!m_delimiter.empty()) {
        query.set_item("delimiter", m_delimiter);
    }
    if (!m_prefix.empty()) {
        query.set_item("prefix", m_prefix);
    }
    if (m_max_keys > 0) {
        query.set_item("max-keys", std::to_string(m_max_keys));
    }

    if (m_is_v2_type) {
        query.set_item("list-type", "2");

        if (!m_continuation_token.empty()) {
            query.set_item("continuation-token", m_continuation_token);
        }
        if (!m_encoding_type.empty()) {
            query.set_item("encoding-type", m_encoding_type);
        }
        if (!m_fetch_owner.empty()) {
            query.set_item("fetch-owner", m_fetch_owner);
        }
        if (!m_start_after.empty()) {
            query.set_item("start-after", m_start_after);
        }
    }
    else {
        if (!m_marker.empty()) {
            query.set_item("marker", m_marker);
        }
    }
}
}  // namespace hestia