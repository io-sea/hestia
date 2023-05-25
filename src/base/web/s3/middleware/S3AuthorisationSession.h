#pragma once

#include "S3AuthorisationObject.h"

#include "HttpRequest.h"
#include "S3Error.h"
#include "UserService.h"

#include <string>
#include <vector>

// Reference:
// https://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html

namespace hestia {
class S3AuthorisationSession {
  public:
    S3AuthorisationSession(UserService* service);

    const S3AuthorisationObject& authorise(const HttpRequest& request);

    void add_chunk(const std::string& chunk);

    bool is_valid() const;

  private:
    void check(const HttpRequest& request);

    bool search_for_user(const HttpRequest& request);

    bool has_signed_payload(const HttpRequest& request) const;

    bool parse_authorisation_info(const HttpRequest& request);

    void parse_signed_headers(const std::string& headers);

    void parse_queries(const HttpRequest& request);

    void parse_credentials(const std::string& credentials);

    std::string extract_string_part(
        const std::string& complete,
        const std::string& identifier,
        const std::string& delimiter) const;

    std::string create_canonical_request(const HttpRequest& request) const;

    std::string create_string_to_sign(
        const HttpRequest& request, const std::string& canonical_request) const;

    std::string get_signature(const std::string& input) const;

    UserService* m_user_service{nullptr};

    S3AuthorisationObject m_object;
};
}  // namespace hestia
