#pragma once

#include "WebApp.h"

namespace hestia {

class DistributedHsmService;
class UserService;

struct HestiaS3WebAppConfig {};

class HestiaS3WebApp : public WebApp {
  public:
    HestiaS3WebApp(
        HestiaS3WebAppConfig config,
        DistributedHsmService* hsm_service,
        UserService* user_service);

  protected:
    void set_up_routing();

    void set_up_middleware();

  private:
    HestiaS3WebAppConfig m_config;
    DistributedHsmService* m_hsm_service{nullptr};
};
}  // namespace hestia