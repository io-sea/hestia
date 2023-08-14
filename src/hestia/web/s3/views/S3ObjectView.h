#pragma once

#include "S3ObjectAdapter.h"
#include "WebView.h"

namespace hestia {
class DistributedHsmService;

class S3ObjectView : public WebView {
  public:
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

    DistributedHsmService* m_service{nullptr};
    std::unique_ptr<S3ObjectAdapter> m_object_adatper;
};
}  // namespace hestia