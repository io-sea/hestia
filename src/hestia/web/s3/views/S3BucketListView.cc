#include "S3BucketListView.h"

#include "DistributedHsmService.h"
#include "S3Responses.h"
#include "S3ViewUtils.h"

#include "HsmService.h"

#include "Logger.h"
#include <sstream>

namespace hestia {
S3BucketListView::S3BucketListView(
    DistributedHsmService* service, const std::string& domain) :
    S3WebView(service, domain),
    m_dataset_adatper(std::make_unique<S3DatasetAdapter>())
{
    LOG_INFO("Loaded S3BucketListView");
}

HttpResponse::Ptr S3BucketListView::on_get(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    LOG_INFO("S3BucketListView:on_get");

    auto container_list_response = m_service->make_request(
        CrudRequest{
            CrudMethod::READ,
            CrudQuery{CrudQuery::BodyFormat::ITEM},
            {auth.m_user_id, auth.m_user_token}},
        HsmItem::dataset_name);
    if (!container_list_response->ok()) {
        const std::string msg =
            container_list_response->get_error().to_string();
        LOG_ERROR(msg);
        return S3ViewUtils::on_server_error(S3Request(request, m_domain), msg);
    }

    auto response = HttpResponse::create();

    S3ListBucketResponse s3_list_bucket_response;
    s3_list_bucket_response.m_owner.m_id           = auth.m_user_id;
    s3_list_bucket_response.m_owner.m_display_name = auth.m_user_display_name;
    m_dataset_adatper->on_list_buckets(
        container_list_response->items(), s3_list_bucket_response);

    response->body() = s3_list_bucket_response.to_string();

    S3ViewUtils::set_common_headers(*response);
    return response;
}
}  // namespace hestia