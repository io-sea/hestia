#pragma once

#include "AuthorizationContext.h"
#include "HttpEvent.h"
#include "HttpRequest.h"

#include <memory>

namespace hestia {
class WebView {
  public:
    using Ptr = std::unique_ptr<WebView>;

    WebView();

    virtual ~WebView() = default;

    HttpResponse::Ptr on_event(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth);

    void set_path(const std::string& path);

    void set_needs_auth(bool needs_auth);

  protected:
    virtual HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth);

    virtual HttpResponse::Ptr on_put(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth);

    virtual HttpResponse::Ptr on_post(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth);

    virtual HttpResponse::Ptr on_delete(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth);

    virtual HttpResponse::Ptr on_head(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth);

    virtual HttpResponse::Ptr on_not_supported(const HttpRequest& request);

    std::string m_path;
    bool m_needs_auth{false};
};
}  // namespace hestia