#pragma once

#include "WebView.h"
#include <memory>

namespace hestia {
class DistributedHsmService;
class HsmObjectJsonAdapter;

class HestiaObjectView : public WebView {
  public:
    HestiaObjectView(DistributedHsmService* hestia_service);

    ~HestiaObjectView();

    HttpResponse::Ptr on_get(const HttpRequest& request) override;

    HttpResponse::Ptr on_put(const HttpRequest& request) override;

    HttpResponse::Ptr on_delete(const HttpRequest& request) override;

    HttpResponse::Ptr on_head(const HttpRequest& request) override;

  private:
    DistributedHsmService* m_hestia_service{nullptr};
    std::unique_ptr<HsmObjectJsonAdapter> m_object_adapter;
};
}  // namespace hestia