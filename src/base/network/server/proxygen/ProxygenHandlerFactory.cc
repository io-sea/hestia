#include "ProxygenHandlerFactory.h"

#ifdef HAVE_PROXYGEN
#include "ProxygenRequestHandler.h"

#include "Logger.h"

namespace hestia {

ProxygenHandlerFactory::ProxygenHandlerFactory(WebApp* web_app) :
    m_web_app(web_app)
{
}

void ProxygenHandlerFactory::onServerStart(folly::EventBase*) noexcept {}

void ProxygenHandlerFactory::onServerStop() noexcept {}

proxygen::RequestHandler* ProxygenHandlerFactory::onRequest(
    proxygen::RequestHandler*, proxygen::HTTPMessage* message) noexcept
{
    LOG_INFO("Request received: " << message->getMethodString());

    auto handler = new ProxygenRequestHandler(m_web_app);
    return handler;
}

}  // namespace hestia
#endif