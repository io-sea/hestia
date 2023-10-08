#include "ProxygenRequestHandler.h"

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
ProxygenRequestHandler::ProxygenRequestHandler(
    WebApp* web_app, std::size_t max_body_size) :
    m_max_body_size(max_body_size), m_web_app(web_app)
{
    LOG_INFO("Created req handler");
    m_request_context = std::make_unique<RequestContext>();
}

void ProxygenRequestHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> msg) noexcept
{
    m_request_context->set_request(ProxygenMessage::to_request(msg.get()));

    const auto method = m_request_context->get_request().get_method_as_string();
    const auto path   = m_request_context->get_request().get_path();
    LOG_INFO(method + " with path: " + path);

    m_web_app->on_event(m_request_context.get(), HttpEvent::HEADERS);
    if (m_request_context->get_response()->get_completion_status()
        == HttpResponse::CompletionStatus::FINISHED) {
        LOG_INFO("Response finished after reading headers - don't need body");
        ProxygenMessage::build_response(
            downstream_, m_request_context->get_response(), true);
        m_response_sent = true;
    }
    else if (m_request_context->get_request()
                 .get_header()
                 .has_expect_continue()) {
        LOG_INFO("Info responding to expect continue");
        auto response =
            HttpResponse::create(HttpStatus(HttpStatus::Code::_100_CONTINUE));
        ProxygenMessage::build_response(downstream_, response.get(), false);
    }
}

void ProxygenRequestHandler::onEgressPaused() noexcept {}

void ProxygenRequestHandler::onEgressResumed() noexcept {}

void ProxygenRequestHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept
{
    LOG_INFO("Got body chunk");
    if (m_request_context->get_response()->get_completion_status()
        == HttpResponse::CompletionStatus::AWAITING_BODY_CHUNK) {
        ReadableBufferView read_buffer(body->data(), body->length());
        auto status = m_request_context->get_stream()->write(read_buffer);
        LOG_INFO("Wrote " << status.m_num_transferred << " bytes to stream");
        if (!status.ok()) {
            LOG_ERROR(
                "Error writing body to stream: " << status.m_state.message());
            ProxygenMessage::build_response(
                downstream_,
                HttpResponse::create(
                    HttpStatus(HttpStatus::Code::_500_INTERNAL_SERVER_ERROR))
                    .get(),
                true);
            m_response_sent = true;
        }
    }
    else {
        LOG_INFO("No input stream - appending chunk to body: ");
        if (m_request_context->get_writeable_request().body().size()
                + body->length()
            < m_max_body_size) {
            m_request_context->get_writeable_request().body() += std::string(
                reinterpret_cast<const char*>(body->data()), body->length());
        }
    }
}

void ProxygenRequestHandler::onEOM() noexcept
{
    if (m_response_sent) {
        LOG_INFO("Already sent eom");
        return;
    }

    LOG_INFO("Got EOM");
    m_web_app->on_event(m_request_context.get(), HttpEvent::EOM);

    if (m_request_context->get_stream()->has_content()) {
        LOG_INFO("Send Initial EOM response");
        ProxygenMessage::build_response(
            downstream_, m_request_context->get_response(), false);

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
        LOG_INFO("Send EOM response");
        ProxygenMessage::build_response(
            downstream_, m_request_context->get_response(), true);
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
    LOG_INFO("Sending " << buffer.length());
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

void ProxygenRequestHandler::on_output_finished(const HttpResponse*)
{
    LOG_INFO("On Output finished");
}

void ProxygenRequestHandler::onUpgrade(proxygen::UpgradeProtocol) noexcept {}

void ProxygenRequestHandler::requestComplete() noexcept
{
    LOG_INFO("Request completed");
    delete this;
}

void ProxygenRequestHandler::onError(proxygen::ProxygenError /*err*/) noexcept
{
    LOG_ERROR("Proxygen server error");
    delete this;
}
}  // namespace hestia
#endif