#pragma once

#include "WebView.h"

namespace hestia {
class S3Service;

class S3ContainerView : public WebView {
  public:
    S3ContainerView(S3Service* service);

    HttpResponse::Ptr on_get(const HttpRequest& request) override;

    virtual HttpResponse::Ptr on_put(const HttpRequest& request) override;

    virtual HttpResponse::Ptr on_delete(const HttpRequest& request) override;

    virtual HttpResponse::Ptr on_head(const HttpRequest& request) override;

  private:
    S3Service* m_service{nullptr};
};
}  // namespace hestia