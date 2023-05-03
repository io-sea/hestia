#pragma once

#include "AuthenticationMiddleware.h"

namespace hestia {
class S3AuthenticationMiddleware : public AuthenticationMiddleware {
  public:
    virtual ~S3AuthenticationMiddleware() = default;
    HttpResponse::Ptr call(
        const HttpRequest& request, responseProviderFunc func) override;
};
}  // namespace hestia