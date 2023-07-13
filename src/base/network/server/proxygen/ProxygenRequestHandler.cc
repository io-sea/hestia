#include "ProxygenRequestHandler.h"

#ifdef HAVE_PROXYGEN

#include "HttpRequest.h"
#include "ProxygenMessage.h"

#include "InMemoryStreamSource.h"
#include "ReadableBufferView.h"
#include "RequestContext.h"

#include "Logger.h"

#include <folly/executors/GlobalExecutor.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include <memory>

namespace hestia {
ProxygenRequestHandler::ProxygenRequestHandler(WebApp* web_app) :
    m_web_app(web_app)
{
}

void ProxygenRequestHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> msg) noexcept
{
    m_request_context = std::make_unique<RequestContext>(
        ProxygenMessage::to_request(msg.get()));

    LOG_INFO(
        "Got request with path: "
        + m_request_context->get_request().get_path());
    LOG_INFO(
        "Got request with method: "
        + m_request_context->get_request().get_method_as_string());

    if (m_request_context->get_request().get_method()
        == HttpRequest::Method::GET) {
        m_web_app->on_request(m_request_context.get());

        if (!m_request_context->get_stream()->waiting_for_content()) {
            LOG_INFO(
                "Sending response, finished ? "
                << m_request_context->finished());
            LOG_INFO(
                "Content length ? " << m_request_context->get_response()
                                           ->header()
                                           .get_content_length());
            ProxygenMessage::build_response(
                downstream_, m_request_context->get_response(), false);
        }

        else if (m_request_context->get_stream()->has_content()) {
            auto event_base = folly::EventBaseManager::get()->getEventBase();
            m_request_context->set_output_chunk_handler(
                [this,
                 event_base](const ReadableBufferView& buffer, bool finished) {
                    on_output_chunk(buffer, finished, event_base);
                    return 0;
                });

            m_request_context->set_output_complete_handler(
                [this](const HttpResponse* response) {
                    on_output_finished(response);
                });

            LOG_INFO("Flushing stream.");
            folly::getGlobalCPUExecutor()->add(
                [this]() { m_request_context->flush_stream(); });
        }
    }
}

void ProxygenRequestHandler::on_output_chunk(
    const ReadableBufferView& buffer, bool finished, folly::EventBase* evb)
{
    LOG_INFO("On output chunk");
    folly::IOBufQueue buf;
    auto data     = buf.preallocate(buffer.length(), buffer.length());
    auto char_buf = reinterpret_cast<char*>(data.first);
    for (std::size_t idx = 0; idx < buffer.length(); idx++) {
        char_buf[idx] = buffer.data()[idx];
    }
    buf.postallocate(buffer.length());

    if (finished) {
        evb->runInEventBaseThread([this, body = buf.move()]() mutable {
            proxygen::ResponseBuilder(downstream_).body(std::move(body)).send();
        });
    }
    else {
        evb->runInEventBaseThread([this, body = buf.move()]() mutable {
            proxygen::ResponseBuilder(downstream_).body(std::move(body)).send();
        });
    }
}

void ProxygenRequestHandler::on_output_finished(const HttpResponse*)
{
    LOG_INFO("On Output finished");
}

void ProxygenRequestHandler::onEgressPaused() noexcept {}

void ProxygenRequestHandler::onEgressResumed() noexcept {}

void ProxygenRequestHandler::onBody(
    std::unique_ptr<folly::IOBuf> buffer) noexcept

{
    if (m_request_context->get_request().get_method()
        == HttpRequest::Method::PUT) {
        if (!buffer->empty()) {
            ReadableBufferView read_buffer(buffer->data(), buffer->length());
            m_request_context->set_body(read_buffer);
            LOG_INFO(
                "Processing body for request: "
                << m_request_context->get_request().to_string());
            m_web_app->on_request(m_request_context.get());
        }
        // TODO: Add full support for input streams
        ProxygenMessage::build_response(
            downstream_, m_request_context->get_response(), false);
    }
}

void ProxygenRequestHandler::onEOM() noexcept
{
    LOG_INFO("Got end of message");
    m_request_context->on_input_complete();
    ProxygenMessage::build_response(downstream_, nullptr, true);
}

void ProxygenRequestHandler::onUpgrade(proxygen::UpgradeProtocol) noexcept {}

void ProxygenRequestHandler::requestComplete() noexcept
{
    LOG_INFO("Request completed");
}

void ProxygenRequestHandler::onError(proxygen::ProxygenError /*err*/) noexcept
{
    LOG_ERROR("Proxygen server error");
}
}  // namespace hestia
#endif