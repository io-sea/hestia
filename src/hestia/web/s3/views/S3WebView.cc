#include "S3WebView.h"

#include "CrudIdentifier.h"
#include "CrudQuery.h"
#include "DistributedHsmService.h"

#include "S3ViewUtils.h"

namespace hestia {
S3WebView::S3WebView(
    DistributedHsmService* hsm_service, const std::string& domain) :
    m_service(hsm_service), m_domain(domain)
{
}

std::pair<HttpResponse::Ptr, CrudResponsePtr> S3WebView::on_get_bucket(
    const S3Request& s3_request,
    const AuthorizationContext& auth,
    bool error_if_not_found,
    const std::string& bucket_name) const
{
    CrudIdentifier bucket_id;
    bucket_id.set_name(
        bucket_name.empty() ? s3_request.get_bucket_name() : bucket_name);
    bucket_id.set_parent_primary_key(auth.m_user_id);

    CrudQuery bucket_query(bucket_id, CrudQuery::BodyFormat::ITEM);
    auto bucket_get_response = m_service->make_request(
        CrudRequest{
            CrudMethod::READ,
            bucket_query,
            {auth.m_user_id, auth.m_user_token}},
        HsmItem::dataset_name);
    if (!bucket_get_response->ok()) {
        const auto msg = bucket_get_response->get_error().to_string();
        LOG_ERROR(msg);
        return {S3ViewUtils::on_server_error(s3_request, msg), nullptr};
    }

    if (!bucket_get_response->found() && error_if_not_found) {
        return {S3ViewUtils::on_no_such_bucket(s3_request), nullptr};
    }
    else {
        return {HttpResponse::create(), std::move(bucket_get_response)};
    }
}
}  // namespace hestia