#pragma once

#include "HttpRequest.h"

#include <memory>

namespace hestia {
class WebView {
  public:
    using Ptr = std::unique_ptr<WebView>;

    virtual ~WebView() = default;

    virtual HttpResponse::Ptr get_response(const HttpRequest& request);

  protected:
    virtual HttpResponse::Ptr on_get(const HttpRequest& request);

    virtual HttpResponse::Ptr on_put(const HttpRequest& request);

    virtual HttpResponse::Ptr on_delete(const HttpRequest& request);

    virtual HttpResponse::Ptr on_head(const HttpRequest& request);

    virtual HttpResponse::Ptr on_not_supported(const HttpRequest& request);
};
}  // namespace hestia