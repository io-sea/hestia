#pragma once

#include "AuthenticationMiddleware.h"

namespace hestia {
class UserService;

class TokenAuthenticationMiddleware : public AuthenticationMiddleware {
  public:
    HttpResponse::Ptr call(
        const HttpRequest& request,
        AuthorizationContext& auth,
        HttpEvent event,
        responseProviderFunc func) override;
};
}  // namespace hestia