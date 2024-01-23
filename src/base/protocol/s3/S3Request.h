#pragma once

#include "AuthorizationContext.h"
#include "Map.h"
#include "S3Path.h"
#include "S3StatusCode.h"
#include "S3Types.h"

#include <string>

namespace hestia {

class XmlElement;

enum class S3UriStyle { VIRTUAL_HOST, PATH };

struct S3UserContext {

    S3UserContext() = default;

    S3UserContext(const AuthorizationContext& auth) :
        m_user_id(auth.m_user_display_name),
        m_user_internal_id(auth.m_user_id),
        m_user_secret_key(auth.m_user_token)
    {
    }

    std::string m_user_id;
    std::string m_user_internal_id;
    std::string m_user_secret_key;
};

class S3Request {

  public:
    enum class Status { UNSET, FAILED, VALID };

    enum class PayloadSignatureType { UNSIGNED, SIGNED };

    S3Request(const std::string& domain);

    // Used by clients
    S3Request(const S3UserContext& user_context, const std::string& domain);

    // Used by server
    S3Request(
        const HttpRequest& req,
        const std::string& domain,
        bool parse_auth = false);

    static void add_aws_namespace(XmlElement& element);

    const std::string& get_bucket_name() const;

    const std::string& get_object_key() const;

    const S3UserContext& get_user_context() const { return m_user_context; }

    void set_user_id(const std::string& id) { m_user_context.m_user_id = id; }

    void set_user_secret_key(const std::string& key)
    {
        m_user_context.m_user_secret_key = key;
    }

    void add_query(const std::pair<std::string, std::string>& query);

    std::string get_resource_host(const std::string& bucket_name) const;

    const std::string& get_domain() const { return m_domain; }

    std::string get_resource_path(
        const std::string& bucket_name,
        const std::string& object_name = {}) const;

    std::string get_signature(
        const HttpRequest& req,
        const std::string& payload_sha256,
        const std::string& secret_key) const;

    void populate_headers(
        const std::string& bucket_name,
        HttpHeader& header,
        const std::string& payload_type = {}) const;

    void populate_authorization_headers(
        PayloadSignatureType payload_sig_type, HttpRequest& req) const;

    std::string to_string() const;

    S3StatusCode get_status_code() const { return m_error_code; }

    void set_extra_headers(const Map& extra_headers)
    {
        m_extra_headers = extra_headers;
    }

    bool is_valid() const { return m_status == Status::VALID; }

    S3Path m_path;
    S3Timestamp m_timestamp;
    S3UriStyle m_uri_style{S3UriStyle::PATH};
    std::string m_endpoint;
    std::string m_region{"us-east-1"};
    S3Range m_range;
    std::string m_tracking_id;
    std::vector<std::string> m_signed_headers;
    std::string m_signature;
    std::string m_payload_type{"application/xml"};

    Map m_extra_headers;
    std::vector<std::pair<std::string, std::string>> m_queries;

  private:
    void parse_authorisation_info(const HttpRequest& request);

    void parse_signed_headers(const std::string& headers);

    void add_signed_header(const std::string& header_key);

    void sort_headers();

    void parse_queries(const HttpRequest& request);

    std::string get_scope() const;

    void sort_queries();

    void parse_credentials(const std::string& credentials);

    void on_error(S3StatusCode error_code);

    std::string create_canonical_request(
        const HttpRequest& request, const std::string& payload_sha256) const;

    std::string create_string_to_sign(
        const HttpRequest& request, const std::string& payload_sha256) const;

    std::string get_signature(
        const std::string& string_to_sign, const std::string& secret_key) const;

    std::string get_signed_headers_flat() const;

    std::string serialize_headers(const hestia::HttpRequest& request) const;

    std::string serialize_queries() const;

    static std::string extract_string_part(
        const std::string& complete,
        const std::string& identifier,
        const std::string& delimiter);

    static std::string to_string(Status status);

    S3UserContext m_user_context;
    Status m_status = Status::UNSET;
    S3StatusCode m_error_code{S3StatusCode::_403_ACCESS_DENIED};
    std::string m_service{"s3"};
    std::string m_domain;
    std::string m_scope_suffix{"aws4_request"};
};

}  // namespace hestia