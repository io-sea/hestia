#pragma once

#include "Server.h"

class TcpServer;
class Socket;

namespace hestia {
class BasicHttpServer : public Server {
  public:
    BasicHttpServer(const Config& config, WebApp* web_app);

    virtual ~BasicHttpServer();

    Status initialize() override;

    Status start() override;

    void wait_until_bound() override;

  private:
    void on_connection(Socket* socket);

    void on_failure(const std::string& reason);

    void receive_until_header_end(std::string& message, Socket* socket);

    std::unique_ptr<TcpServer> m_tcp_server;
};
}  // namespace hestia