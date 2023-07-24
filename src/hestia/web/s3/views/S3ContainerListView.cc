#include "S3ContainerListView.h"

#include "S3Service.h"
#include "S3ViewUtils.h"

#include "HsmService.h"

#include "Logger.h"
#include <sstream>

namespace hestia {
S3ContainerListView::S3ContainerListView(S3Service* service) :
    m_service(service), m_dataset_adatper(std::make_unique<S3DatasetAdapter>())
{
    LOG_INFO("Loaded S3ContainerListView");
}

HttpResponse::Ptr S3ContainerListView::on_get(
    const HttpRequest& request, const User&)
{
    (void)request;
    (void)m_service;

    /*
    LOG_INFO("S3ContainerListView:on_get");
    auto hsm_service = m_service->get_hsm_service();

    auto container_list_response = hsm_service->make_request(
        {HsmItem::Type::DATASET,
         std::make_unique<CrudRequest>(CrudMethod::READ)});
    if (!container_list_response->ok()) {
        LOG_ERROR(container_list_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    auto response = HttpResponse::create();
    std::string body;
    m_dataset_adatper->on_list(container_list_response->items(), body);
    response->set_body(body);
    */

    auto response = HttpResponse::create();
    return response;
}
}  // namespace hestia