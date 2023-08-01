#include "WebView.h"

#include "Logger.h"

namespace hestia {

WebView::WebView() {}
void WebView::set_path(const std::string& path)
{
    m_path = path;
}

HttpResponse::Ptr WebView::get_response(
    const HttpRequest& request, const User& user)
{
    switch (request.get_method()) {
        case HttpRequest::Method::GET:
            return on_get(request, user);
        case HttpRequest::Method::PUT:
            return on_put(request, user);
        case HttpRequest::Method::POST:
            return on_post(request, user);
        case HttpRequest::Method::DELETE:
            return on_delete(request, user);
        case HttpRequest::Method::HEAD:
            return on_head(request, user);
        case HttpRequest::Method::UNSET:
        default:
            return on_not_supported(request);
    }
}

HttpResponse::Ptr WebView::on_get(const HttpRequest& request, const User&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_post(const HttpRequest& request, const User&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_put(const HttpRequest& request, const User&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_delete(const HttpRequest& request, const User&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_head(const HttpRequest& request, const User&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_not_supported(const HttpRequest& request)
{
    const auto msg =
        "Method " + request.get_method_as_string() + " is not supported.";
    LOG_INFO(
        "Got unsupported method: " + request.get_path() + " "
        + request.get_method_as_string());
    LOG_INFO(request.get_header().to_string());
    LOG_INFO(request.body());
    return HttpResponse::create(400, "Bad method", msg);
}
}  // namespace hestia