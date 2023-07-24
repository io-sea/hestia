#pragma once

#include "HestiaApplication.h"

#include "ServerConfig.h"

namespace hestia {
class HestiaServer : public HestiaApplication {
  public:
    HestiaServer();

    OpStatus run() override;

  private:
    void set_app_mode() override;
};
}  // namespace hestia