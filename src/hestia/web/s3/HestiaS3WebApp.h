#pragma once

#include "WebApp.h"

namespace hestia {

class HsmS3Service;
class UserService;

struct HestiaS3WebAppConfig {};

class HestiaS3WebApp : public WebApp {
  public:
    HestiaS3WebApp(
        HestiaS3WebAppConfig config,
        HsmS3Service* s3_service,
        UserService* user_service);

  protected:
    void set_up_routing();

    void set_up_middleware();

  private:
    HestiaS3WebAppConfig m_config;
    HsmS3Service* m_s3_service{nullptr};
};
}  // namespace hestia