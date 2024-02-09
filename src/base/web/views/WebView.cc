#include "WebView.h"

#include "Logger.h"

namespace hestia {

WebView::WebView() {}
void WebView::set_path(const std::string& path)
{
    m_path = path;
}

HttpResponse::Ptr WebView::on_event(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    switch (request.get_method()) {
        case HttpRequest::Method::GET:
            return on_get(request, event, auth);
        case HttpRequest::Method::PUT:
            return on_put(request, event, auth);
        case HttpRequest::Method::POST:
            return on_post(request, event, auth);
        case HttpRequest::Method::DELETE:
            return on_delete(request, event, auth);
        case HttpRequest::Method::HEAD:
            return on_head(request, event, auth);
        case HttpRequest::Method::OPTIONS:
            return on_options(request, event, auth);
        case HttpRequest::Method::UNSET:
        default:
            return on_not_supported(request);
    }
}

HttpResponse::Ptr WebView::on_options(
    const HttpRequest&, HttpEvent, const AuthorizationContext&)
{
    auto response = HttpResponse::create({HttpStatus::Code::_200_OK});
    response->header().set_item("Access-Control-Allow-Methods", "*");
    response->header().set_item("Access-Control-Allow-Headers", "*");
    return response;
}

HttpResponse::Ptr WebView::on_get(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_post(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_put(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_delete(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
{
    return on_not_supported(request);
}

HttpResponse::Ptr WebView::on_head(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
{
    return on_not_supported(request);
}

void WebView::set_needs_auth(bool needs_auth)
{
    m_needs_auth = needs_auth;
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
    return HttpResponse::create({HttpStatus::Code::_400_BAD_REQUEST, msg});
}
}  // namespace hestia