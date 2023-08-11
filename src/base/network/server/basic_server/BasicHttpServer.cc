#include "BasicHttpServer.h"

#include "BufferView.h"
#include "ReadableBufferView.h"
#include "WriteableBufferView.h"

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "RequestContext.h"

#include "Socket.h"
#include "TcpServer.h"

#include "Logger.h"

#include <string>

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

HttpResponse::Ptr write_to_stream(const HttpRequest& req, Stream* stream)
{
    auto response = HttpResponse::create();
    if (!req.body().empty()) {
        auto status = stream->write(ReadableBufferView(req.body()));
        LOG_INFO("Wrote " << status.m_num_transferred << " bytes to stream");
        if (!status.ok()) {
            LOG_ERROR(
                "Error writing body to stream: " << status.m_state.message());
            response = HttpResponse::create(
                HttpError(HttpError::Code::_500_INTERNAL_SERVER_ERROR));
        }
    }
    return response;
}

void BasicHttpServer::receive_until_header_end(
    std::string& message, Socket* socket)
{
    while (message.rfind("\r\n\r\n") == message.npos) {
        socket->respond(
            HttpResponse::create(HttpError(HttpError::Code::_100_CONTINUE))
                ->to_string());
        message.append(socket->recieve());
    }
}

void BasicHttpServer::on_connection(Socket* socket)
{
    auto message = socket->recieve();

    if (socket->connected()) {
        receive_until_header_end(message, socket);
        LOG_INFO("Got message: " + message);

        HttpRequest request(message);
        RequestContext request_context(request);

        auto content_length_string = request.get_header().get_content_length();
        std::size_t content_length = 0;
        if (!content_length_string.empty()) {
            content_length = std::stoul(content_length_string);
        }
        // Set up stream if the requested view supports this
        if (m_web_app->get_streamable(
                request_context.get_request().get_path())) {
            m_web_app->on_request(&request_context);

            //  Handle input (outstanding request body)
            // Not supported: streamed body and streamed response
            if (request_context.get_stream()->waiting_for_content()) {
                std::size_t total_received = request.body().size();

                // Handle initial body if parsed
                auto status =
                    write_to_stream(request, request_context.get_stream());
                if (status->error()) {
                    socket->respond(status->to_string());
                }
                while (total_received < content_length) {
                    // Ask for more data
                    socket->respond(
                        HttpResponse::create(
                            HttpError(HttpError::Code::_100_CONTINUE))
                            ->to_string());

                    // Get further data
                    request.body().assign(socket->recieve());
                    LOG_INFO("Got " << request.body().size() << " extra bytes.")
                    total_received += request.body().size();

                    // Push body chunk to stream
                    auto status =
                        write_to_stream(request, request_context.get_stream());
                    if (status->error()) {
                        socket->respond(status->to_string());
                        break;
                    }
                }
                // Reset response & respond based on stream status
                request_context.set_response(nullptr);
                request_context.on_input_complete();
            }

            LOG_INFO(
                "Sending response: "
                << request_context.get_response()->to_string());
            socket->respond(request_context.get_response()->to_string());
        }
        // Non-streamed input
        else {
            while (request.body().size() < content_length) {
                socket->respond(HttpResponse::create(
                                    HttpError(HttpError::Code::_100_CONTINUE))
                                    ->to_string());
                request.body().append(socket->recieve());
                LOG_INFO("Got " << request.body().size() << " total bytes.")
            }
            request_context.set_request(request);
            // Process with full body
            m_web_app->on_request(&request_context);

            LOG_INFO(
                "Sending response: "
                << request_context.get_response()->to_string());
            socket->respond(request_context.get_response()->to_string());
        }

        // Handle streamed response
        if (request_context.get_stream()->has_content()) {
            request_context.set_output_chunk_handler(
                [&socket](
                    const hestia::ReadableBufferView& buffer, bool finished) {
                    (void)finished;
                    socket->respond(
                        std::string(buffer.data(), buffer.length()));
                    return buffer.length();
                });
            LOG_INFO("Responding with provided stream");
            request_context.flush_stream();
        }

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