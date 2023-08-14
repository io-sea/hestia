#pragma once

#include "S3ObjectAdapter.h"
#include "WebView.h"

namespace hestia {
class DistributedHsmService;

class S3ObjectView : public WebView {
  public:
    struct ObjectContext {
        std::string m_id;
        std::size_t m_size{0};
    };

    S3ObjectView(DistributedHsmService* service);

    HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext&) override;

    HttpResponse::Ptr on_put(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext&) override;

    HttpResponse::Ptr on_delete(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext&) override;

    HttpResponse::Ptr on_head(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext&) override;

  private:
    HttpResponse::Ptr on_get_or_head(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth,
        ObjectContext& object_context);

    DistributedHsmService* m_service{nullptr};
    std::unique_ptr<S3ObjectAdapter> m_object_adatper;
};
}  // namespace hestia