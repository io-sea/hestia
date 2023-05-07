#pragma once

#include "S3Service.h"

#include "WebApp.h"

#include <filesystem>

namespace hestia {
class ObjectStoreClient;

struct S3WebAppConfig {
    std::filesystem::path m_object_store_path;
};

class S3WebApp : public WebApp {
  public:
    S3WebApp(
        ObjectStoreClient* object_store,
        const S3WebAppConfig& config          = {},
        std::unique_ptr<S3Service> s3_service = nullptr);

    virtual ~S3WebApp() = default;

  protected:
    void set_up_routing();

    void set_up_middleware();

    S3WebAppConfig m_config;
    ObjectStoreClient* m_object_store_client{nullptr};
    std::unique_ptr<S3Service> m_s3_service;
};
}  // namespace hestia