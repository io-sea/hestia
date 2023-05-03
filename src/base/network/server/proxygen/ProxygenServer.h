#pragma once

#include "Server.h"

#ifdef HAVE_PROXYGEN
namespace proxygen {
class HTTPServer;
}

namespace hestia {
class ProxygenServer : public Server {
  public:
    ProxygenServer(const Server::Config& config, WebApp* web_app);

    ~ProxygenServer();

    Status initialize() override;

    Status start() override;

  private:
    std::unique_ptr<proxygen::HTTPServer> m_impl;
};
}  // namespace hestia
#endif