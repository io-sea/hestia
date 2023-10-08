#pragma once

#include "UrlRouter.h"

#include <memory>

namespace hestia {
class S3ObjectView;
class S3BucketView;
class S3BucketListView;

class S3UrlRouter : public UrlRouter {
  public:
    virtual WebView* get_view(const std::string& path) override;

    void set_object_view(std::unique_ptr<S3ObjectView> view);

    void set_bucket_view(std::unique_ptr<S3BucketView> view);

    void set_bucket_list_view(std::unique_ptr<S3BucketListView> view);

  private:
    std::unique_ptr<S3ObjectView> m_object_view;
    std::unique_ptr<S3BucketView> m_bucket_view;
    std::unique_ptr<S3BucketListView> m_bucket_list_view;
};
}  // namespace hestia