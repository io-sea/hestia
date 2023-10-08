#pragma once

#include "S3HsmObjectAdapter.h"
#include "S3WebView.h"

namespace hestia {

class DistributedHsmService;
class CrudResponse;
using CrudResponsePtr = std::unique_ptr<CrudResponse>;

class S3ObjectView : public S3WebView {
  public:
    struct ObjectContext {
        std::string m_id;
        std::size_t m_size{0};
    };

    S3ObjectView(DistributedHsmService* service);

    HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext& auth) override;

    HttpResponse::Ptr on_put(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext& auth) override;

    HttpResponse::Ptr on_delete(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext& auth) override;

    HttpResponse::Ptr on_head(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext& auth) override;

  private:
    HttpResponse::Ptr on_get_or_head(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth,
        bool is_get);

    HttpResponse::Ptr on_copy_object(
        const S3Request& s3_request,
        const HttpRequest& request,
        const AuthorizationContext& auth,
        const std::string& copy_source);

    HttpResponse::Ptr on_put_object(
        const S3Request& s3_request,
        const HttpRequest& request,
        const AuthorizationContext& auth);

    HttpResponse::Ptr on_get_data(
        const S3Request& s3_request,
        const HttpRequest& request,
        const AuthorizationContext& auth,
        const std::string& object_id);

    HttpResponse::Ptr on_put_data(
        const HttpRequest& request,
        const AuthorizationContext& auth,
        const std::string& object_id,
        std::size_t content_length);

    std::pair<HttpResponse::Ptr, CrudResponsePtr> on_get_object(
        const S3Request& req,
        const AuthorizationContext& auth,
        bool error_if_not_found        = true,
        const std::string& object_key  = {},
        const std::string& bucket_name = {}) const;

    HttpResponse::Ptr on_delete_object(
        const S3Request& req,
        const AuthorizationContext& auth,
        const std::string& key) const;

    std::unique_ptr<S3HsmObjectAdapter> m_object_adatper;
};
}  // namespace hestia