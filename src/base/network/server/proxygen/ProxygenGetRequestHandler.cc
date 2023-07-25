#include "ProxygenGetRequestHandler.h"

#ifdef HAVE_PROXYGEN

#include "HttpRequest.h"
#include "HttpResponse.h"
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
ProxygenGetRequestHandler::ProxygenGetRequestHandler(WebApp* web_app) :
    ProxygenRequestHandler(web_app)
{
}

void ProxygenGetRequestHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> msg) noexcept
{
    ProxygenRequestHandler::onRequest(std::move(msg));

    // Get initial response
    m_web_app->on_request(m_request_context.get());
    LOG_INFO(
        "Sending response: " << m_request_context->get_response()->to_string());
    ProxygenMessage::build_response(
        downstream_, m_request_context->get_response(), false);
}

void ProxygenGetRequestHandler::onEOM() noexcept
{
    // Connect output function to stream if we have a source
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

        // Handle streamed response
        LOG_INFO("Flushing stream.");
        folly::getGlobalCPUExecutor()->add(
            [this]() { m_request_context->flush_stream(); });
    }
    else {
        ProxygenMessage::build_response(downstream_, nullptr, true);
    }
}

void ProxygenGetRequestHandler::on_output_chunk(
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

void ProxygenGetRequestHandler::on_output_finished(const HttpResponse*)
{
    LOG_INFO("On Output finished");
}
}  // namespace hestia
#endif