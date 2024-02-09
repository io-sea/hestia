#pragma once

#include "Server.h"

class TcpServer;
class Socket;

namespace hestia {
class BasicHttpServer : public Server {
  public:
    BasicHttpServer(const BaseServerConfig& config, WebApp* web_app);

    virtual ~BasicHttpServer();

    Status initialize() override;

    Status start() override;

    void wait_until_bound() override;

  private:
    void on_connection(Socket* socket);

    bool on_head(RequestContext& context, Socket* socket) const;

    bool on_body_chunk(
        RequestContext& context,
        Socket* socket,
        HttpEvent& last_event,
        std::size_t expected_body_size,
        std::size_t& body_count) const;

    void on_failure(const std::string& reason);

    void receive_until_header_end(std::string& message, Socket* socket);

    std::unique_ptr<TcpServer> m_tcp_server;
};
}  // namespace hestia