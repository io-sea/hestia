#pragma once

#include "UrlRouter.h"

#include <memory>

namespace hestia {
class S3ObjectView;
class S3ContainerView;
class S3ContainerListView;

class S3UrlRouter : public UrlRouter {
  public:
    virtual WebView* get_view(const std::string& path) override;

    void set_object_view(std::unique_ptr<S3ObjectView> view);

    void set_container_view(std::unique_ptr<S3ContainerView> view);

    void set_container_list_view(std::unique_ptr<S3ContainerListView> view);

  private:
    std::unique_ptr<S3ObjectView> m_object_view;
    std::unique_ptr<S3ContainerView> m_container_view;
    std::unique_ptr<S3ContainerListView> m_container_list_view;
};
}  // namespace hestia