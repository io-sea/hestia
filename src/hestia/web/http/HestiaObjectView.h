#pragma once

#include "WebView.h"
#include <memory>

namespace hestia {
class HestiaService;

class HestiaObjectView : public WebView {
  public:
    HestiaObjectView(HestiaService* hestia_service);

    HttpResponse::Ptr on_get(const HttpRequest& request) override;

    HttpResponse::Ptr on_put(const HttpRequest& request) override;

    HttpResponse::Ptr on_delete(const HttpRequest& request) override;

    HttpResponse::Ptr on_head(const HttpRequest& request) override;

  private:
    HestiaService* m_hestia_service{nullptr};
};
}  // namespace hestia