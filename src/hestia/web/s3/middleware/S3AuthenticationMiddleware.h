#pragma once

#include "AuthenticationMiddleware.h"

namespace hestia {
class S3AuthenticationMiddleware : public AuthenticationMiddleware {
  public:
    S3AuthenticationMiddleware(
        const std::string& domain, bool enable_auth = true);

    virtual ~S3AuthenticationMiddleware() = default;
    HttpResponse::Ptr call(
        const HttpRequest& request,
        AuthorizationContext& user,
        HttpEvent event,
        responseProviderFunc func) override;

  private:
    bool m_enable_auth{true};
    std::string m_domain;
};
}  // namespace hestia