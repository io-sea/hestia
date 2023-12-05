#include "S3ListObjectsRequest.h"

namespace hestia {

S3ListObjectsRequest::S3ListObjectsRequest(
    const S3UserContext& user_context, const std::string& domain) :
    m_s3_request(user_context, domain)
{
}

S3ListObjectsRequest::S3ListObjectsRequest(
    const HttpRequest& req, const std::string& domain) :
    m_s3_request(req, domain)
{
}

void S3ListObjectsRequest::build_query()
{
    if (!m_delimiter.empty()) {
        m_s3_request.add_query({"delimiter", m_delimiter});
    }
    if (!m_prefix.empty()) {
        m_s3_request.add_query({"prefix", m_prefix});
    }
    if (m_max_keys > 0) {
        m_s3_request.add_query({"max-keys", std::to_string(m_max_keys)});
    }

    if (m_is_v2_type) {
        m_s3_request.add_query({"list-type", "2"});

        if (!m_continuation_token.empty()) {
            m_s3_request.add_query(
                {"continuation-token", m_continuation_token});
        }
        if (!m_encoding_type.empty()) {
            m_s3_request.add_query({"encoding-type", m_encoding_type});
        }
        if (!m_fetch_owner.empty()) {
            m_s3_request.add_query({"fetch-owner", m_fetch_owner});
        }
        if (!m_start_after.empty()) {
            m_s3_request.add_query({"start-after", m_start_after});
        }
    }
    else {
        if (!m_marker.empty()) {
            m_s3_request.add_query({"marker", m_marker});
        }
    }
}
}  // namespace hestia