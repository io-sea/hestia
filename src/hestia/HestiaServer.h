#pragma once

#include "HestiaApplication.h"

#include "ServerConfig.h"

namespace hestia {
class HestiaServer : public HestiaApplication {
  public:
    HestiaServer();

    OpStatus run() override;

  private:
    void set_app_mode(const std::string& host, unsigned port) override;
};
}  // namespace hestia