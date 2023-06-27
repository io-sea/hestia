#pragma once

#include "WebView.h"

#include <memory>

namespace hestia {
class DistributedHsmService;

class HestiaNodeView : public WebView {
  public:
    HestiaNodeView(DistributedHsmService* hestia_service);

    HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_put(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_delete(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_head(
        const HttpRequest& request, const User& user) override;

  private:
    DistributedHsmService* m_hestia_service{nullptr};
};
}  // namespace hestia