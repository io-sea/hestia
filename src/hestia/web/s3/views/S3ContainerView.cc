#include "S3ContainerView.h"

#include "DistributedHsmService.h"
#include "S3DatasetAdapter.h"
#include "S3Path.h"
#include "S3ViewUtils.h"

#include "HsmService.h"
#include "TypedCrudRequest.h"

#include "Logger.h"

#include <iostream>
#include <sstream>

namespace hestia {
S3ContainerView::S3ContainerView(DistributedHsmService* service) :
    m_service(service), m_dataset_adatper(std::make_unique<S3DatasetAdapter>())
{
    LOG_INFO("Loaded S3ContainerView");
}

HttpResponse::Ptr S3ContainerView::on_get(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    const auto s3_path = S3Path(request.get_path());

    CrudQuery query(
        KeyValuePair{"name", s3_path.m_container_name}, CrudQuery::Format::GET,
        CrudQuery::OutputFormat::ITEM);
    const auto get_response = m_service->make_request(
        CrudRequest{query, {auth.m_user_id, auth.m_user_token}},
        HsmItem::dataset_name);

    if (!get_response->ok()) {
        LOG_ERROR(get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (!get_response->found()) {
        return HttpResponse::create(404, "Not Found");
    }

    auto dataset = get_response->get_item_as<Dataset>();

    auto response = HttpResponse::create();
    m_dataset_adatper->on_list(*dataset, response->body());
    response->header().set_item(
        "Content-Length", std::to_string(response->body().size()));

    return response;
}

HttpResponse::Ptr S3ContainerView::on_put(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    const auto s3_path = S3Path(request.get_path());

    CrudIdentifier id;
    id.set_name(s3_path.m_container_name);

    CrudQuery query(
        KeyValuePair{"name", s3_path.m_container_name}, CrudQuery::Format::GET,
        CrudQuery::OutputFormat::ITEM);
    const auto get_response = m_service->make_request(
        CrudRequest{query, {auth.m_user_id, auth.m_user_token}},
        HsmItem::dataset_name);

    if (!get_response->ok()) {
        LOG_ERROR(get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (!get_response->found()) {
        auto create_response = m_service->make_request(
            CrudRequest{
                CrudMethod::CREATE, {auth.m_user_id, auth.m_user_token}, {id}},
            HsmItem::dataset_name);
        if (!create_response->ok()) {
            LOG_ERROR(create_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error");
        }
    }
    else {
        Dataset dataset;
        dataset.set_name(s3_path.m_container_name);

        auto update_response = m_service->make_request(
            TypedCrudRequest<Dataset>(
                CrudMethod::UPDATE, dataset,
                {auth.m_user_id, auth.m_user_token}),
            HsmItem::dataset_name);
        if (!update_response->ok()) {
            LOG_ERROR(update_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error");
        }
    }

    return HttpResponse::create(201, "Created");
}

HttpResponse::Ptr S3ContainerView::on_delete(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
{
    (void)request;
    /*
    auto hsm_service = m_service->get_hsm_service();

    const auto s3_path = S3Path(request.get_path());

    Dataset dataset;
    dataset.set_name(s3_path.m_container_name);

    auto remove_response =
        hsm_service->make_request({dataset, HsmServiceRequestMethod::RELEASE});
    if (!remove_response->ok()) {
        LOG_ERROR(remove_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }
    */
    return {};
}

HttpResponse::Ptr S3ContainerView::on_head(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
{
    (void)request;
    /*
    auto hsm_service = m_service->get_hsm_service();

    const auto s3_path = S3Path(request.get_path());

    Dataset dataset;
    dataset.set_name(s3_path.m_container_name);

    auto get_response =
        hsm_service->make_request({dataset, HsmServiceRequestMethod::GET});
    if (!get_response->ok()) {
        LOG_ERROR(get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (!get_response->object_found()) {
        return HttpResponse::create(404, "Not Found");
    }
    */

    auto response = HttpResponse::create();

    // S3ViewUtils::metadata_to_header(container.m_user_metadata,
    // response.get());
    return response;
}
}  // namespace hestia