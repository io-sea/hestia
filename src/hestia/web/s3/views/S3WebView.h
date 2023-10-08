#pragma once

#include "S3Request.h"
#include "WebView.h"

namespace hestia {
class DistributedHsmService;
class CrudResponse;
using CrudResponsePtr = std::unique_ptr<CrudResponse>;

class S3WebView : public WebView {
  public:
    S3WebView(DistributedHsmService* hsm_service);

  protected:
    std::pair<HttpResponse::Ptr, CrudResponsePtr> on_get_bucket(
        const S3Request& req,
        const AuthorizationContext& auth,
        bool error_if_not_found        = true,
        const std::string& bucket_name = {}) const;

    DistributedHsmService* m_service{nullptr};
};
}  // namespace hestia