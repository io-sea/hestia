#include "HestiaObjectView.h"

#include "HestiaService.h"

namespace hestia {
HestiaObjectView::HestiaObjectView(HestiaService* hestia_service) :
    WebView(), m_hestia_service(hestia_service)
{
    (void)m_hestia_service;
}

HttpResponse::Ptr HestiaObjectView::on_get(const HttpRequest& request)
{
    (void)request;

    auto response = HttpResponse::create();
    response->set_body("Hello world");
    return response;
}

HttpResponse::Ptr HestiaObjectView::on_put(const HttpRequest& request)
{
    (void)request;
    auto response = HttpResponse::create();
    response->set_body("Hello world");
    return response;
}

HttpResponse::Ptr HestiaObjectView::on_delete(const HttpRequest& request)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaObjectView::on_head(const HttpRequest& request)
{
    (void)request;
    return HttpResponse::create();
}

}  // namespace hestia