#include "BasicHttpServer.h"

#include "RequestContext.h"

#include "Socket.h"
#include "TcpServer.h"

#include "Logger.h"

namespace hestia {
BasicHttpServer::BasicHttpServer(const Config& config, WebApp* web_app) :
    Server(config, web_app), m_tcp_server(std::make_unique<TcpServer>())
{
    if (m_config.m_block_on_launch) {
        m_tcp_server->set_block_on_listen(true);
    }
}

BasicHttpServer::~BasicHttpServer()
{
    LOG_INFO("Shutting down");
}

BasicHttpServer::Status BasicHttpServer::initialize()
{
    return {};
}

BasicHttpServer::Status BasicHttpServer::start()
{
    TcpServer::Address socket_address;
    socket_address.m_host = m_config.m_ip;
    socket_address.m_port = m_config.m_http_port;
    // socket_address.mPrefix = mListenAddress.mPrefix;

    LOG_INFO(
        "Starting server on " << socket_address.m_host << " with port "
                              << socket_address.m_port);

    auto on_connection = [this](Socket* socket) {
        this->on_connection(socket);
    };

    auto on_failure = [this](const ConnectionResult& result) {
        this->on_failure(
            result.m_error_message
            + " | Error code: " + std::to_string(result.m_error_code));
    };

    m_tcp_server->listen(socket_address, on_connection, on_failure);
    return {};
}

void BasicHttpServer::on_connection(Socket* socket)
{
    auto message = socket->recieve();
    LOG_INFO("Got message: " + message);

    if (socket->connected()) {
        HttpRequest request(message);
        std::string extra_bytes;
        if (request.is_content_outstanding()) {
            LOG_INFO(
                "Waiting for more content: "
                << request.get_header().get_content_length());
            extra_bytes += socket->recieve();
        }

        RequestContext request_context(request);
        m_web_app->on_request(&request_context);

        if (!request.body().empty()) {
            auto status = request_context.write_to_stream(
                ReadableBufferView(request.body()));
            if (!status.ok()) {
                LOG_ERROR(
                    "Error writing body to stream: "
                    << status.m_state.message());
            }
        }

        socket->respond(request_context.get_response()->to_string());
        socket->close();
    }
    else if (socket->closed()) {
        LOG_INFO("Client closed connection");
    }
    else {
        LOG_ERROR("Connection error");
    }
}

void BasicHttpServer::wait_until_bound()
{
    m_tcp_server->wait_until_bound();
}

void BasicHttpServer::on_failure(const std::string& reason)
{
    LOG_ERROR("Connection failed with reason: " << reason);
}
}  // namespace hestia