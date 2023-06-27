#pragma once

#include "StorageTier.h"
#include "StringAdapter.h"
#include "WebView.h"
#include <memory>

namespace hestia {
class DistributedHsmService;
class StorageTierJsonAdapter;

class HestiaTierView : public WebView {
  public:
    HestiaTierView(DistributedHsmService* hestia_service);

    ~HestiaTierView();

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
    std::unique_ptr<JsonAdapter<StorageTier>> m_tier_adapter;
};
}  // namespace hestia