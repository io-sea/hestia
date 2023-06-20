#pragma once

#include "WebView.h"

namespace hestia {
class S3Service;

class S3ContainerListView : public WebView {
  public:
    S3ContainerListView(S3Service* service);

    HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user) override;

  private:
    S3Service* m_service{nullptr};
};
}  // namespace hestia