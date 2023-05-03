#pragma once

#include "HttpRequest.h"

#include <functional>
#include <memory>
#include <vector>

namespace hestia {
class UserService;
class ApplicationMiddleware;
class RequestContext;
class UrlRouter;
class Database;
class WebView;

class WebApp {
  public:
    using Ptr = std::unique_ptr<WebApp>;

    WebApp(Database* db = nullptr);

    static Ptr create(Database* db = nullptr);

    virtual ~WebApp();

    void add_middleware(std::unique_ptr<ApplicationMiddleware> middleware);

    virtual void on_request(RequestContext* request_context) const noexcept;

    void set_url_router(std::unique_ptr<UrlRouter> router);

  protected:
    virtual HttpResponse::Ptr on_view_not_found(
        const HttpRequest& request) const;

    HttpResponse::Ptr on_middleware_layer(
        WebView* view,
        std::size_t working_idx,
        const HttpRequest& request) const;

    std::vector<std::unique_ptr<ApplicationMiddleware>> m_middleware;
    std::unique_ptr<UrlRouter> m_url_router;

    std::unique_ptr<UserService> m_user_service;
    std::size_t m_body_chunk_size{4000};
};
}  // namespace hestia