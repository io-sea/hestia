#pragma once

#include "HestiaApplication.h"

#include "ServerConfig.h"

namespace hestia {
class Server;

class HestiaServer : public HestiaApplication {
  public:
    HestiaServer();

    virtual ~HestiaServer();

    OpStatus run() override;

  private:
    void set_app_mode(const std::string& host, unsigned port) override;

    std::unique_ptr<Server> m_server;
};
}  // namespace hestia