#include "S3ContainerView.h"

#include "S3DatasetAdapter.h"
#include "S3Path.h"
#include "S3Service.h"
#include "S3ViewUtils.h"

#include "HsmService.h"

#include "Logger.h"

#include <iostream>
#include <sstream>

namespace hestia {
S3ContainerView::S3ContainerView(S3Service* service) :
    m_service(service), m_dataset_adatper(std::make_unique<S3DatasetAdapter>())
{
    LOG_INFO("Loaded S3ContainerView");
}

HttpResponse::Ptr S3ContainerView::on_get(const HttpRequest&, const User&)
{
    /*
    auto hsm_service = m_service->get_hsm_service();

    const auto s3_path = S3Path(request.get_path());

    CrudQuery query(KeyValuePair{"name", s3_path.m_container_name});

    auto get_response = hsm_service->make_request(
        {HsmItem::Type::DATASET, std::make_unique<CrudRequest>(query)});
    if (!get_response->ok()) {
        LOG_ERROR(get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (!get_response->found()) {
        return HttpResponse::create(404, "Not Found");
    }

    auto response = HttpResponse::create();

    // S3ViewUtils::metadata_to_header(container.m_user_metadata,
    // response.get());



    std::string body;
    m_dataset_adatper->on_list(get_response->items(), body);

    response->set_body(body);
    response->header().set_item(
        "Content-Length", std::to_string(response->body().size()));

        */
    auto response = HttpResponse::create();
    return response;
}

HttpResponse::Ptr S3ContainerView::on_put(
    const HttpRequest& request, const User&)
{
    (void)request;
    /*
    auto hsm_service = m_service->get_hsm_service();

    const auto s3_path = S3Path(request.get_path());

    Dataset dataset;
    dataset.set_name(s3_path.m_container_name);

    auto exists_response =
        hsm_service->make_request({dataset, HsmServiceRequestMethod::EXISTS});
    if (!exists_response->ok()) {
        LOG_ERROR(exists_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (exists_response->object_found()) {
        return HttpResponse::create(409, "Conflict");
    }
    */
    // S3ViewUtils::header_to_metadata(request, container.m_user_metadata);

    /*
    auto put_response =
        hsm_service->make_request({dataset, HsmServiceRequestMethod::UPDATE});
    if (!put_response->ok()) {
        LOG_ERROR(put_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }
    */

    // FAIL_CHECK(m_service->put(container));

    return HttpResponse::create(201, "Created");
}

HttpResponse::Ptr S3ContainerView::on_delete(
    const HttpRequest& request, const User&)
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
    const HttpRequest& request, const User&)
{
    (void)request;
    (void)m_service;
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