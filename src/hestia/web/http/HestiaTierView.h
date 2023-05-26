#pragma once

#include "WebView.h"
#include <memory>

namespace hestia {
class DistributedHsmService;

class HestiaTierView : public WebView {
  public:
    HestiaTierView(DistributedHsmService* hestia_service);

    HttpResponse::Ptr on_get(const HttpRequest& request) override;

    HttpResponse::Ptr on_put(const HttpRequest& request) override;

    HttpResponse::Ptr on_delete(const HttpRequest& request) override;

    HttpResponse::Ptr on_head(const HttpRequest& request) override;

  private:
    DistributedHsmService* m_hestia_service{nullptr};
};
}  // namespace hestia