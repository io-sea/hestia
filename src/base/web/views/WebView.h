#pragma once

#include "HttpRequest.h"
#include "User.h"

#include <memory>

namespace hestia {
class WebView {
  public:
    using Ptr = std::unique_ptr<WebView>;

    WebView();

    virtual ~WebView() = default;

    virtual HttpResponse::Ptr get_response(
        const HttpRequest& request, const User& = {});

    void set_path(const std::string& path);

    bool can_stream() const { return m_can_stream; }

  protected:
    virtual HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user);

    virtual HttpResponse::Ptr on_put(
        const HttpRequest& request, const User& user);

    virtual HttpResponse::Ptr on_post(
        const HttpRequest& request, const User& user);

    virtual HttpResponse::Ptr on_delete(
        const HttpRequest& request, const User& user);

    virtual HttpResponse::Ptr on_head(
        const HttpRequest& request, const User& user);

    virtual HttpResponse::Ptr on_not_supported(const HttpRequest& request);

    std::string m_path;
    bool m_can_stream = false;
};
}  // namespace hestia