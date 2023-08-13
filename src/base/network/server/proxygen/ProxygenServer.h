#pragma once

#include "Server.h"
#include "ThreadCollection.h"
#include <condition_variable>
#include <memory>


#ifdef HAVE_PROXYGEN
namespace proxygen {
class HTTPServer;
}
namespace folly {
class Init;
}

namespace hestia {
class ProxygenServer : public Server {
  public:
    ProxygenServer(const Server::Config& config, WebApp* web_app);

    virtual ~ProxygenServer();

    Status initialize() override;

    Status start() override;

    Status stop() override;

    void wait_until_bound() override;

  private:
    ThreadCollection m_threads;
    std::condition_variable m_bound_cv;
    std::mutex m_bound_mutex;

    std::unique_ptr<proxygen::HTTPServer> m_impl;
};
}  // namespace hestia
#endif