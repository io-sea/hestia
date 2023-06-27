#pragma once

#include "S3DatasetAdapter.h"
#include "WebView.h"

namespace hestia {
class S3Service;

class S3ContainerView : public WebView {
  public:
    S3ContainerView(S3Service* service);

    HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user) override;

    virtual HttpResponse::Ptr on_put(
        const HttpRequest& request, const User& user) override;

    virtual HttpResponse::Ptr on_delete(
        const HttpRequest& request, const User& user) override;

    virtual HttpResponse::Ptr on_head(
        const HttpRequest& request, const User& user) override;

  private:
    S3Service* m_service{nullptr};
    std::unique_ptr<S3DatasetAdapter> m_dataset_adatper;
};
}  // namespace hestia