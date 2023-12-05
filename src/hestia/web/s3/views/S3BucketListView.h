#pragma once

#include "S3DatasetAdapter.h"
#include "S3WebView.h"

namespace hestia {
class DistributedHsmService;

class S3BucketListView : public S3WebView {
  public:
    S3BucketListView(DistributedHsmService* service, const std::string& domain);

    HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;

  private:
    std::unique_ptr<S3DatasetAdapter> m_dataset_adatper;
};
}  // namespace hestia