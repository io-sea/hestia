#pragma once

#include "ServerConfig.h"
#include "WebApp.h"

namespace hestia {

class DistributedHsmService;

class HestiaWebAppConfig {
  public:
    std::string m_static_resource_dir;
    bool m_cache_static_resources{true};
};

class HestiaWebApp : public WebApp {
  public:
    HestiaWebApp(
        DistributedHsmService* hestia_service, HestiaWebAppConfig config = {});
};
}  // namespace hestia