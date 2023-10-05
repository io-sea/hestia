#pragma once

#include "HttpRequest.h"
#include "S3Status.h"

#include <vector>

namespace hestia {
class S3AuthorisationObject {
  public:
    enum class Status { UNSET, WAITING_FOR_PAYLOAD, FAILED, VALID };

    void add_chunk(const std::string& chunk);

    void add_signed_header(const std::string& header);

    void add_query(const std::pair<std::string, std::string>& query);

    std::string get_credential_header(const HttpRequest& req) const;

    std::string get_signature(const HttpRequest& req) const;

    const S3Status& get_error() const;

    bool is_initialized() const;

    bool is_valid() const;

    bool is_waiting_for_payload() const;

    void on_error(const hestia::S3Status& error);

    void on_waiting_for_payload();

    void set_is_valid();

    void sort_headers();

    void sort_queries();

    bool parse_authorisation_info(const HttpRequest& request);

    std::string serialize_headers(const hestia::HttpRequest& request) const;

    std::string serialize_queries() const;

    std::string to_string() const;

    static std::string to_string(Status status);

    std::string m_user_identifier;
    std::string m_user_internal_id;
    std::string m_user_key;
    std::string m_date;
    std::string m_region;
    std::vector<std::string> m_signed_headers;

    std::string m_signature;
    std::string m_payload;

  private:
    std::string create_canonical_request(const HttpRequest& request) const;

    std::string create_string_to_sign(const HttpRequest& request) const;

    std::string get_signature(const std::string& string_to_sign) const;

    std::string get_scope() const;

    void parse_signed_headers(const std::string& headers);

    void parse_queries(const HttpRequest& request);

    void parse_credentials(const std::string& credentials);

    static std::string extract_string_part(
        const std::string& complete,
        const std::string& identifier,
        const std::string& delimiter);

    using Query = std::pair<std::string, std::string>;
    std::vector<Query> m_queries;

    Status m_status = Status::UNSET;
    S3Status m_error{S3Status::Code::_403_ACCESS_DENIED, {}};

    std::string m_service{"s3"};
    std::string m_scope_suffix{"aws4_request"};
};
}  // namespace hestia