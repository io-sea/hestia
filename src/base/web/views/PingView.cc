#include "PingView.h"


namespace hestia {

HttpResponse::Ptr PingView::on_get(
    const HttpRequest&, HttpEvent, const AuthorizationContext&)
{
    return hestia::HttpResponse::create();
}

HttpResponse::Ptr PingView::on_head(
    const HttpRequest&, HttpEvent, const AuthorizationContext&)
{
    return hestia::HttpResponse::create();
}

}  // namespace hestia