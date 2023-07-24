#include "S3ObjectView.h"

#include "S3Path.h"
#include "S3Service.h"
#include "S3ViewUtils.h"

#include "HsmObject.h"
#include "HsmService.h"
#include "RequestContext.h"

#include "Logger.h"

#include <iostream>
#include <sstream>

namespace hestia {
S3ObjectView::S3ObjectView(S3Service* service) :
    m_service(service), m_object_adatper(std::make_unique<S3ObjectAdapter>())
{
    LOG_INFO("Loaded object view");
}

HttpResponse::Ptr S3ObjectView::on_get(const HttpRequest& request, const User&)
{
    (void)request;

    /*
    auto hsm_service = m_service->get_hsm_service();

    const auto s3_path = S3Path(request.get_path());

    Dataset container;
    container.set_name(s3_path.m_container_name);

    auto container_get_response =
        hsm_service->make_request({container, HsmServiceRequestMethod::GET});
    if (!container_get_response->ok()) {
        LOG_ERROR(container_get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    HsmObject hsm_object;
    hsm_object.set_name(s3_path.m_object_id);

    auto obj_get_response =
        hsm_service->make_request({hsm_object, HsmServiceRequestMethod::GET});
    if (!obj_get_response->ok()) {
        LOG_ERROR(obj_get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (!obj_get_response->object_found()) {
        return HttpResponse::create(404, "Not Found");
    }

    auto data_get_response = hsm_service->make_request(
        {hsm_object, HsmServiceRequestMethod::GET_DATA},
        request.get_context()->get_stream());
    if (!data_get_response->ok()) {
        LOG_ERROR(data_get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    auto response = HttpResponse::create();

    Map metadata;
    m_object_adatper->get_headers(
        container_get_response->dataset(), obj_get_response->object(),
        metadata);

    auto on_item =
        [&response](const std::string& key, const std::string& value) {
            response->header().set_item(S3Path::meta_prefix + key, value);
        };
    metadata.for_each_item(on_item);
    */
    auto response = HttpResponse::create();
    return response;
}

HttpResponse::Ptr S3ObjectView::on_head(const HttpRequest& request, const User&)
{
    (void)request;

    /*
    auto hsm_service = m_service->get_hsm_service();

    const auto s3_path = S3Path(request.get_path());

    Dataset container;
    container.set_name(s3_path.m_container_name);
    auto container_get_response =
        hsm_service->make_request({container, HsmServiceRequestMethod::GET});
    if (!container_get_response->ok()) {
        LOG_ERROR(container_get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    HsmObject hsm_object;
    hsm_object.set_name(s3_path.m_object_id);

    auto obj_get_response =
        hsm_service->make_request({hsm_object, HsmServiceRequestMethod::GET});
    if (!obj_get_response->ok()) {
        LOG_ERROR(obj_get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (!obj_get_response->object_found()) {
        return HttpResponse::create(404, "Not Found");
    }

    auto response = HttpResponse::create();

    Map metadata;
    m_object_adatper->get_headers(
        container_get_response->dataset(), obj_get_response->object(),
        metadata);

    auto on_item =
        [&response](const std::string& key, const std::string& value) {
            response->header().set_item(S3Path::meta_prefix + key, value);
        };
    metadata.for_each_item(on_item);
    */
    auto response = HttpResponse::create();
    return response;
}

HttpResponse::Ptr S3ObjectView::on_put(const HttpRequest& request, const User&)
{
    (void)request;

    /*
    auto hsm_service = m_service->get_hsm_service();

    const auto s3_path = S3Path(request.get_path());

    Dataset container;
    container.set_name(s3_path.m_container_name);

    auto container_get_response =
        hsm_service->make_request({container, HsmServiceRequestMethod::GET});
    if (!container_get_response->ok()) {
        LOG_ERROR(container_get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    HsmObject hsm_object;
    hsm_object.set_name(s3_path.m_object_id);

    // object.m_user_metadata =
    //     request.get_header().get_items_with_prefix(S3Path::meta_prefix);

    const auto content_length = request.get_header().get_content_length();
    hsm_object.set_size(
        content_length.empty() ? 0 : std::stoul(content_length));
    hsm_object.set_dataset_id(container_get_response->dataset().id());

    auto object_put_response = hsm_service->make_request(
        {hsm_object, HsmServiceRequestMethod::CREATE},
        request.get_context()->get_stream());
    if (!object_put_response->ok()) {
        LOG_ERROR(object_put_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }
    */
    return HttpResponse::create(201, "Created");
}

HttpResponse::Ptr S3ObjectView::on_delete(
    const HttpRequest& request, const User&)
{
    (void)request;
    /*
    const auto s3_path = S3Path(request.get_path());

    auto hsm_service = m_service->get_hsm_service();

    Dataset container;
    container.set_name(s3_path.m_container_name);

    auto container_get_response =
        hsm_service->make_request({container, HsmServiceRequestMethod::GET});
    if (!container_get_response->ok()) {
        LOG_ERROR(container_get_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    HsmObject hsm_object;
    hsm_object.set_name(s3_path.m_object_id);

    auto obj_get_reponse =
        hsm_service->make_request({hsm_object, HsmServiceRequestMethod::GET});
    if (!obj_get_reponse->ok()) {
        LOG_ERROR(obj_get_reponse->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    if (!obj_get_reponse->object_found()) {
        return HttpResponse::create(404, "Not Found");
    }

    auto remove_response = hsm_service->make_request(
        {hsm_object, HsmServiceRequestMethod::RELEASE});
    if (!remove_response->ok()) {
        LOG_ERROR(remove_response->get_error().to_string());
        return HttpResponse::create(500, "Internal Server Error");
    }

    */
    return HttpResponse::create();
}
}  // namespace hestia