#pragma once

#include "AuthorizationContext.h"
#include "HttpEvent.h"
#include "HttpRequest.h"

#include <functional>
#include <memory>
#include <vector>

namespace hestia {
class UserService;
class ApplicationMiddleware;
class RequestContext;
class UrlRouter;
class WebView;

class WebApp {
  public:
    using Ptr = std::unique_ptr<WebApp>;

    WebApp(UserService* user_service);

    static Ptr create(UserService* user_service);

    virtual ~WebApp();

    void add_middleware(std::unique_ptr<ApplicationMiddleware> middleware);

    virtual void on_event(
        RequestContext* request_context, HttpEvent event) const noexcept;

    void set_url_router(std::unique_ptr<UrlRouter> router);

    void set_access_control_origin(const std::string& val)
    {
      m_access_control_origin = val;
    }

  protected:
    virtual HttpResponse::Ptr on_view_not_found(
        const HttpRequest& request) const;

    HttpResponse::Ptr on_middleware_layer(
        WebView* view,
        AuthorizationContext& auth,
        std::size_t working_idx,
        const HttpRequest& request,
        HttpEvent event) const;

    void log_event(const HttpRequest req, HttpEvent event) const;

    std::vector<std::unique_ptr<ApplicationMiddleware>> m_middleware;
    std::unique_ptr<UrlRouter> m_url_router;

    UserService* m_user_service{nullptr};
    std::size_t m_body_chunk_size{4000};
    std::string m_access_control_origin;
};
}  // namespace hestia