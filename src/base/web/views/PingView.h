#pragma once

#include "WebView.h"

namespace hestia {
class PingView : public WebView {
  public:
    HttpResponse::Ptr on_get(const HttpRequest& request, const User&) override;

    HttpResponse::Ptr on_head(const HttpRequest& request, const User&) override;
};
}  // namespace hestia