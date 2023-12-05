#include "S3BucketView.h"

#include "DistributedHsmService.h"
#include "S3DatasetAdapter.h"
#include "S3ListObjectsRequest.h"
#include "S3Responses.h"
#include "S3Status.h"
#include "S3ViewUtils.h"

#include "HsmService.h"
#include "TypedCrudRequest.h"

#include "Logger.h"

#include <iostream>
#include <sstream>

namespace hestia {
S3BucketView::S3BucketView(
    DistributedHsmService* service, const std::string& domain) :
    S3WebView(service, domain),
    m_dataset_adapter(std::make_unique<S3DatasetAdapter>()),
    m_object_adapter(std::make_unique<S3HsmObjectAdapter>())
{
    LOG_INFO("Loaded S3BucketView");
}

// List Objects endpoint
HttpResponse::Ptr S3BucketView::on_get(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    LOG_INFO("S3BucketView:on_get");

    S3ListObjectsRequest list_object_request(request, m_domain);

    auto [status, get_bucket_response] =
        on_get_bucket(list_object_request.m_s3_request, auth);
    if (status->error()) {
        return std::move(status);
    }

    auto response = HttpResponse::create();
    auto dataset  = get_bucket_response->get_item_as<Dataset>();

    S3ListObjectsResponse s3_list_objects_response;
    m_object_adapter->on_list_objects(*dataset, s3_list_objects_response);
    response->body() = s3_list_objects_response.to_string();

    S3ViewUtils::set_common_headers(*response);
    return response;
}

HttpResponse::Ptr S3BucketView::on_head(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    LOG_INFO("S3BucketView:on_head");

    S3Request s3_request(request, m_domain);
    auto [status, _] = on_get_bucket(s3_request, auth);

    return std::move(status);
}

// Create bucket endpoint
HttpResponse::Ptr S3BucketView::on_put(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    S3Request s3_request(request, m_domain);

    auto [status, get_bucket_response] = on_get_bucket(s3_request, auth, false);
    if (status->error()) {
        return std::move(status);
    }

    if (!get_bucket_response->found()) {
        CrudIdentifier id;
        id.set_name(s3_request.m_path.m_bucket_name);
        id.set_parent_primary_key(auth.m_user_id);

        auto create_response = m_service->make_request(
            CrudRequest{
                CrudMethod::CREATE, {id}, {auth.m_user_id, auth.m_user_token}},
            HsmItem::dataset_name);
        if (!create_response->ok()) {
            const auto msg = create_response->get_error().to_string();
            LOG_ERROR(msg);
            return S3ViewUtils::on_server_error(s3_request, msg);
        }
    }
    else {
        return S3ViewUtils::on_bucket_already_exists(s3_request);
    }
    return HttpResponse::create(201, "Created");
}

HttpResponse::Ptr S3BucketView::on_delete(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    LOG_INFO("S3BucketView:on_delete");

    S3Request s3_request(request, m_domain);

    auto [status, get_bucket_response] = on_get_bucket(s3_request, auth, false);
    if (status->error()) {
        return std::move(status);
    }

    if (get_bucket_response->found()) {
        auto dataset = get_bucket_response->get_item_as<Dataset>();
        if (!dataset->objects().empty()) {
            return S3ViewUtils::on_tried_to_delete_nonempty_bucket(s3_request);
        }

        auto remove_response = m_service->make_request(
            CrudRequest{
                CrudMethod::REMOVE,
                {get_bucket_response->get_item()->get_primary_key()},
                {auth.m_user_id, auth.m_user_token}},
            HsmItem::dataset_name);
        if (!remove_response->ok()) {
            const auto msg = remove_response->get_error().to_string();
            LOG_ERROR(msg);
            return S3ViewUtils::on_server_error(s3_request, msg);
        }
    }
    return HttpResponse::create(HttpStatus::Code::_204_NO_CONTENT);
}

}  // namespace hestia