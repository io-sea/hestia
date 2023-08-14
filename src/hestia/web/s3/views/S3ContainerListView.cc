#include "S3ContainerListView.h"

#include "DistributedHsmService.h"
#include "S3ViewUtils.h"

#include "HsmService.h"

#include "Logger.h"
#include <sstream>

namespace hestia {
S3ContainerListView::S3ContainerListView(DistributedHsmService* service) :
    m_service(service), m_dataset_adatper(std::make_unique<S3DatasetAdapter>())
{
    LOG_INFO("Loaded S3ContainerListView");
}

HttpResponse::Ptr S3ContainerListView::on_get(
    const HttpRequest&, HttpEvent, const AuthorizationContext& auth)
{
    LOG_INFO("S3ContainerListView:on_get");

    auto container_list_response = m_service->make_request(
        CrudRequest{
            CrudQuery{CrudQuery::OutputFormat::ITEM},
            {auth.m_user_id, auth.m_user_token}},
        HsmItem::dataset_name);

    if (!container_list_response->ok()) {
        LOG_ERROR(container_list_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    auto response = HttpResponse::create();
    m_dataset_adatper->on_list(
        container_list_response->items(), response->body());
    return response;
}
}  // namespace hestia