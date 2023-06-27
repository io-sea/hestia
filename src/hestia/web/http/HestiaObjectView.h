#pragma once

#include "HsmObject.h"
#include "StringAdapter.h"
#include "WebView.h"

#include <memory>

namespace hestia {
class DistributedHsmService;

class HestiaObjectView : public WebView {
  public:
    HestiaObjectView(DistributedHsmService* hestia_service);

    ~HestiaObjectView();

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
    std::unique_ptr<JsonAdapter<HsmObject>> m_object_adapter;
};
}  // namespace hestia