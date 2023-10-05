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

bool BasicHttpServer::on_head(
    RequestContext& request_context, Socket* socket) const
{
    LOG_INFO(
        "Got headers: "
        << request_context.get_request().get_header().to_string());

    m_web_app->on_event(&request_context, HttpEvent::HEADERS);
    if (request_context.get_response()->get_completion_status()
        == HttpResponse::CompletionStatus::FINISHED) {
        LOG_INFO("Response finished after reading headers - don't want body");
        socket->respond(request_context.get_response()->to_string());
        return true;
    }
    else if (request_context.get_request().get_header().has_expect_continue()) {
        LOG_INFO("Info responding to expect continue");
        socket->respond(
            HttpResponse::create(HttpStatus(HttpStatus::Code::_100_CONTINUE))
                ->to_string());
    }
    return false;
}

bool BasicHttpServer::on_body_chunk(
    RequestContext& context,
    Socket* socket,
    HttpEvent& last_event,
    std::size_t expected_body_size,
    std::size_t& body_count) const
{
    LOG_INFO("On body chunk");

    IOResult write_result;
    if (last_event == HttpEvent::HEADERS
        && !context.get_request().body().empty()
        && context.get_request().body().size() <= expected_body_size) {
        write_result = context.get_stream()->write(
            ReadableBufferView(context.get_request().body()));
    }
    else {
        write_result =
            context.get_stream()->write(ReadableBufferView(socket->recieve()));
    }

    if (!write_result.ok()) {
        LOG_ERROR("Failed to write to stream ");
        socket->respond(
            HttpResponse::create(
                HttpStatus(HttpStatus::Code::_500_INTERNAL_SERVER_ERROR))
                ->to_string());
        return true;
    }

    body_count += write_result.m_num_transferred;
    last_event = HttpEvent::BODY;

    LOG_INFO(
        "Body count is: " << body_count
                          << " expected_body_size is: " << expected_body_size);

    if (body_count >= expected_body_size) {
        LOG_INFO("Finished with streamed body - sending eom");

        // if (context.get_stream()->waiting_for_content())
        //{
        LOG_INFO("Resetting stream");
        auto reset_state = context.get_stream()->reset();
        if (!reset_state.ok()) {
            LOG_ERROR("Error resetting stream: " << reset_state.to_string());
            socket->respond(
                HttpResponse::create(
                    HttpStatus(HttpStatus::Code::_500_INTERNAL_SERVER_ERROR))
                    ->to_string());
            return true;
        }
        //}

        m_web_app->on_event(&context, HttpEvent::EOM);
        socket->respond(context.get_response()->to_string());
        return true;
    }
    return false;
}

void BasicHttpServer::on_connection(Socket* socket)
{
    RequestContext request_context;
    HttpEvent last_event{HttpEvent::CONNECTED};

    std::size_t received_body_count{0};
    std::size_t expected_body_size{0};
    while (socket->connected()) {
        if (last_event == HttpEvent::CONNECTED) {
            request_context.get_writeable_request().on_chunk(socket->recieve());

            if (request_context.get_request().has_read_header()) {
                const bool should_disconnect = on_head(request_context, socket);
                if (should_disconnect) {
                    break;
                }
                last_event = HttpEvent::HEADERS;
            }
        }
        else {
            if (last_event == HttpEvent::HEADERS) {
                expected_body_size = request_context.get_request()
                                         .get_header()
                                         .get_content_length_as_size_t();
            }

            if (expected_body_size == 0) {
                m_web_app->on_event(&request_context, HttpEvent::EOM);
                socket->respond(request_context.get_response()->to_string());
                if (request_context.get_stream()->has_content()) {
                    LOG_INFO("Responding with stream");
                    request_context.set_output_chunk_handler(
                        [&socket](
                            const hestia::ReadableBufferView& buffer,
                            bool finished) {
                            (void)finished;
                            socket->respond(
                                std::string(buffer.data(), buffer.length()));
                            return buffer.length();
                        });
                    request_context.flush_stream();
                }
                break;
            }
            else if (
                request_context.get_response()->get_completion_status()
                == HttpResponse::CompletionStatus::AWAITING_BODY_CHUNK) {
                const bool should_disconnect = on_body_chunk(
                    request_context, socket, last_event, expected_body_size,
                    received_body_count);
                if (should_disconnect) {
                    break;
                }
            }
            else {
                if (request_context.get_request().body().size()
                    < expected_body_size) {
                    request_context.get_writeable_request().body() +=
                        socket->recieve();
                }

                if (request_context.get_writeable_request().body().size()
                    >= expected_body_size) {
                    LOG_INFO("Finished with body - sending eom");

                    if (request_context.get_stream()->waiting_for_content()) {
                        auto reset_state =
                            request_context.get_stream()->reset();
                        if (!reset_state.ok()) {
                            LOG_ERROR(
                                "Error resetting stream: "
                                << reset_state.to_string());
                            socket->respond(
                                HttpResponse::create(
                                    HttpStatus(HttpStatus::Code::
                                                   _500_INTERNAL_SERVER_ERROR))
                                    ->to_string());
                            break;
                        }
                    }
                    m_web_app->on_event(&request_context, HttpEvent::EOM);
                    socket->respond(
                        request_context.get_response()->to_string());
                    break;
                }
            }
        }
    }
    socket->close();
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