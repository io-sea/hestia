#pragma once

#include "S3DatasetAdapter.h"
#include "WebView.h"

namespace hestia {
class S3Service;

class S3ContainerListView : public WebView {
  public:
    S3ContainerListView(S3Service* service);

    HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;

  private:
    S3Service* m_service{nullptr};
    std::unique_ptr<S3DatasetAdapter> m_dataset_adatper;
};
}  // namespace hestia