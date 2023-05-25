#pragma once

#include "HttpRequest.h"
#include "S3Error.h"

#include <vector>

namespace hestia {
class S3AuthorisationObject {
  public:
    enum class Status { UNSET, WAITING_FOR_PAYLOAD, FAILED, VALID };

    void add_chunk(const std::string& chunk);

    void add_signed_header(const std::string& header);

    void add_query(const std::pair<std::string, std::string>& query);

    bool is_initialized() const;

    bool is_valid() const;

    bool is_waiting_for_payload() const;

    void on_error(const hestia::S3Error& error);

    void on_waiting_for_payload();

    void set_is_valid();

    void sort_headers();

    void sort_queries();

    std::string serialize_headers(const hestia::HttpRequest& request) const;

    std::string serialize_queries() const;

    std::string to_string() const;

    static std::string to_string(Status status);

    std::string m_user_identifier;
    std::string m_user_key;
    std::string m_date;
    std::string m_region;

    std::string m_signature;
    std::string m_payload;

  private:
    std::vector<std::string> m_signed_headers;
    using Query = std::pair<std::string, std::string>;
    std::vector<Query> m_queries;

    Status m_status = Status::UNSET;
    S3Error m_error{S3Error::Code::_403_ACCESS_DENIED, ""};
};
}  // namespace hestia