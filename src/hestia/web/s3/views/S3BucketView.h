#pragma once

#include "S3DatasetAdapter.h"
#include "S3HsmObjectAdapter.h"
#include "S3WebView.h"

namespace hestia {
class DistributedHsmService;

class S3BucketView : public S3WebView {
  public:
    S3BucketView(DistributedHsmService* service);

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
    std::unique_ptr<S3DatasetAdapter> m_dataset_adapter;
    std::unique_ptr<S3HsmObjectAdapter> m_object_adapter;
};
}  // namespace hestia