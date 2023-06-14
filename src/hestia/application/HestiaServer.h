#pragma once

#include "ServerConfig.h"

namespace hestia {
class HestiaServer {
  public:
    HestiaServer(const ServerConfig& config);

    void run();

  private:
    ServerConfig m_config;
};
}  // namespace hestia