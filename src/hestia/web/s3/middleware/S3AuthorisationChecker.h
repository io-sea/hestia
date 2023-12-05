#pragma once

#include "HttpRequest.h"
#include "S3Request.h"
#include "S3Status.h"
#include "UserService.h"

#include <vector>

// Reference:
// https://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html

namespace hestia {
class S3AuthorisationChecker {
  public:
    enum class Status { UNSET, WAITING_FOR_PAYLOAD, FAILED, VALID };

    struct UserContext {
        std::string m_id;
        std::string m_token;
    };

    struct AuthResponse {
        Status m_status;
        S3Status m_s3_status;
        UserContext m_user;
    };

    static AuthResponse authorise(
        const UserService& user_service,
        const HttpRequest& request,
        const std::string& domain);

  private:
    static S3Status check(
        const S3Request& s3_request, const HttpRequest& request);

    static std::pair<S3Status, UserContext> find_user(
        const UserService& user_service, S3Request& s3_request);

    static bool awaiting_signed_payload(const HttpRequest& request);
};
}  // namespace hestia
