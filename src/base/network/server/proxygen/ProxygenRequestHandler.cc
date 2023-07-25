#include "ProxygenRequestHandler.h"

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
    LOG_INFO(m_request_context->get_request().to_string());
}

void ProxygenRequestHandler::onEgressPaused() noexcept {}

void ProxygenRequestHandler::onEgressResumed() noexcept {}

void ProxygenRequestHandler::onBody(std::unique_ptr<folly::IOBuf>) noexcept

{
    LOG_INFO("Got body");
}

void ProxygenRequestHandler::onEOM() noexcept
{
    LOG_INFO("Got end of message");
    ProxygenMessage::build_response(downstream_, nullptr, true);
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