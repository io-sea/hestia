#include "HestiaTierView.h"

#include "HestiaService.h"

namespace hestia {
HestiaTierView::HestiaTierView(HestiaService* hestia_service) :
    WebView(), m_hestia_service(hestia_service)
{
    (void)m_hestia_service;
}

HttpResponse::Ptr HestiaTierView::on_get(const HttpRequest& request)
{
    (void)request;

    auto response = HttpResponse::create();
    response->set_body("Hello world");
    return response;
}

HttpResponse::Ptr HestiaTierView::on_put(const HttpRequest& request)
{
    (void)request;
    auto response = HttpResponse::create();
    response->set_body("Hello world");
    return response;
}

HttpResponse::Ptr HestiaTierView::on_delete(const HttpRequest& request)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaTierView::on_head(const HttpRequest& request)
{
    (void)request;
    return HttpResponse::create();
}

}  // namespace hestia