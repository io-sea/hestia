#pragma once

#include "S3DatasetAdapter.h"
#include "WebView.h"

namespace hestia {
class DistributedHsmService;

class S3ContainerView : public WebView {
  public:
    S3ContainerView(DistributedHsmService* service);

    HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext&) override;

    virtual HttpResponse::Ptr on_put(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext&) override;

    virtual HttpResponse::Ptr on_delete(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext&) override;

    virtual HttpResponse::Ptr on_head(
        const HttpRequest& request,
        HttpEvent,
        const AuthorizationContext&) override;

  private:
    DistributedHsmService* m_service{nullptr};
    std::unique_ptr<S3DatasetAdapter> m_dataset_adatper;
};
}  // namespace hestia