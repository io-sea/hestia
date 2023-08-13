#pragma once

#include "WebView.h"

namespace hestia {
class PingView : public WebView {
  public:
    HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;

    HttpResponse::Ptr on_head(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;
};
}  // namespace hestia