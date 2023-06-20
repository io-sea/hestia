#pragma once

#include "WebView.h"
#include <memory>

namespace hestia {
class DatasetService;
class DatasetJsonAdapter;

class HestiaDatasetView : public WebView {
  public:
    HestiaDatasetView(DatasetService* dataset_service);

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
    DatasetService* m_dataset_service{nullptr};
    std::unique_ptr<DatasetJsonAdapter> m_dataset_adapter;
};
}  // namespace hestia