#pragma once

#include "ServerConfig.h"

namespace hestia {
class HestiaService {
  public:
    HestiaService(const ServerConfig& config);

    void run();

  private:
    ServerConfig m_config;
};
}  // namespace hestia