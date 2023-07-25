#include "ProxygenPutRequestHandler.h"

#ifdef HAVE_PROXYGEN

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ProxygenMessage.h"

#include "InMemoryStreamSource.h"
#include "ReadableBufferView.h"
#include "RequestContext.h"

#include "Logger.h"

#include <memory>

namespace hestia {
ProxygenPutRequestHandler::ProxygenPutRequestHandler(WebApp* web_app) :
    ProxygenRequestHandler(web_app)
{
}
void ProxygenPutRequestHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> msg) noexcept
{
    ProxygenRequestHandler::onRequest(std::move(msg));

    // Get initial response if expecting a stream
    if (m_web_app->get_streamable(
            m_request_context->get_request().get_path())) {
        m_web_app->on_request(m_request_context.get());
        LOG_INFO(
            "Sending response: "
            << m_request_context->get_response()->to_string());
        ProxygenMessage::build_response(
            downstream_, m_request_context->get_response(), false);
    }
}

void ProxygenPutRequestHandler::onBody(
    std::unique_ptr<folly::IOBuf> buffer) noexcept

{
    ReadableBufferView read_buffer(
        std::move(buffer->data()), std::move(buffer->length()));
    if (m_request_context->get_stream()->waiting_for_content()) {
        LOG_INFO("Got " << read_buffer.length() << " extra bytes.")

        // Request more data
        LOG_INFO(
            "Sending response: "
            << m_request_context->get_response()->to_string());
        ProxygenMessage::build_response(
            downstream_, m_request_context->get_response(), false);

        // Push body chunk to stream
        auto status = m_request_context->get_stream()->write(read_buffer);
        LOG_INFO(
            "Wrote " << status.m_num_transferred
                     << " bytes to internal buffer");
        if (!status.ok()) {
            LOG_ERROR(
                "Error writing body to stream: " << status.m_state.message());
            ProxygenMessage::build_response(
                downstream_,
                HttpResponse::create(
                    HttpError(HttpError::Code::_500_INTERNAL_SERVER_ERROR))
                    .get(),
                true);
        }
    }
    else {
        m_body.append(std::string(read_buffer.data(), read_buffer.length()));
    }
}

void ProxygenPutRequestHandler::onEOM() noexcept
{
    LOG_INFO("Got end of message");
    // Reset response
    m_request_context->set_response(nullptr);

    // Non-streamed body
    if (!m_body.empty()) {
        auto req   = m_request_context->get_request();
        req.body() = m_body;
        m_request_context->set_request(req);
        m_web_app->on_request(
            m_request_context.get());  // Get finished response
    }
    m_request_context->on_input_complete();

    ProxygenMessage::build_response(
        downstream_, m_request_context->get_response(), true);
}
}  // namespace hestia
#endif