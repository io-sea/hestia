#pragma once

#include "WebView.h"
#include <memory>

namespace hestia {
class DistributedHsmService;
class DatasetJsonAdapter;

class HestiaDatasetView : public WebView {
  public:
    HestiaDatasetView(DistributedHsmService* hsm_service);

    ~HestiaDatasetView();

    HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_put(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_delete(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_head(
        const HttpRequest& request, const User& user) override;

  private:
    DistributedHsmService* m_hsm_service{nullptr};
    std::unique_ptr<DatasetJsonAdapter> m_dataset_adapter;
};
}  // namespace hestia