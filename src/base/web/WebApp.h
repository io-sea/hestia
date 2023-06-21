#pragma once

#include "HttpRequest.h"
#include "User.h"

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

    virtual void on_request(RequestContext* request_context) const noexcept;

    void set_url_router(std::unique_ptr<UrlRouter> router);

  protected:
    virtual HttpResponse::Ptr on_view_not_found(
        const HttpRequest& request) const;

    HttpResponse::Ptr on_middleware_layer(
        WebView* view,
        User& user,
        std::size_t working_idx,
        const HttpRequest& request) const;

    std::vector<std::unique_ptr<ApplicationMiddleware>> m_middleware;
    std::unique_ptr<UrlRouter> m_url_router;

    UserService* m_user_service{nullptr};
    std::size_t m_body_chunk_size{4000};
};
}  // namespace hestia