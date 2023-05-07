#include "ProxygenRequestHandler.h"

#include "ProxygenMessage.h"

#include "Logger.h"
#include "RequestContext.h"

#ifdef HAVE_PROXYGEN
#include <folly/executors/GlobalExecutor.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/ResponseBuilder.h>

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

    m_web_app->on_request(m_request_context.get());

    if (!m_request_context->get_stream()->waiting_for_content()) {
        LOG_INFO(
            "Sending response, finished ? " << m_request_context->finished());
        LOG_INFO(
            "Content length ? " << m_request_context->get_response()
                                       ->header()
                                       .get_content_length());
        ProxygenMessage::build_response(
            downstream_, m_request_context->get_response(),
            m_request_context->finished());
    }

    if (m_request_context->get_stream()->has_content()) {
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
            proxygen::ResponseBuilder(downstream_)
                .body(std::move(body))
                .sendWithEOM();
        });
    }
    else {
        evb->runInEventBaseThread([this, body = buf.move()]() mutable {
            proxygen::ResponseBuilder(downstream_).body(std::move(body)).send();
        });
    }
}

void ProxygenRequestHandler::on_output_finished(const HttpResponse* response)
{
    (void)response;
    LOG_INFO("On Output finished");
    // ProxygenMessage::buildResponse(downstream_, response, true);
}

void ProxygenRequestHandler::onEgressPaused() noexcept {}

void ProxygenRequestHandler::onEgressResumed() noexcept {}

void ProxygenRequestHandler::onBody(
    std::unique_ptr<folly::IOBuf> buffer) noexcept
{
    ReadableBufferView read_buffer(buffer->data(), buffer->length());
    LOG_INFO("Got body: " << read_buffer.data());
    auto status = m_request_context->write_to_stream(read_buffer);
    if (!status.ok()) {
        LOG_ERROR("Error writing body to stream: " << status.m_state.message());
    }
}

void ProxygenRequestHandler::onEOM() noexcept
{
    LOG_INFO("Got end of message");
    // mRequestContext->onInputComplete();
    if (!m_request_context->finished()) {
        // ProxygenMessage::buildResponse(downstream_,
        // mRequestContext->getResponse(), true);
    }
}

void ProxygenRequestHandler::onUpgrade(proxygen::UpgradeProtocol) noexcept {}

void ProxygenRequestHandler::requestComplete() noexcept
{
    // m_finished = true;
    // m_paused   = true;
}

void ProxygenRequestHandler::onError(proxygen::ProxygenError /*err*/) noexcept
{
    // m_finished = true;
    // m_paused   = true;
}
}  // namespace hestia
#endif