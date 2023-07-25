#include "ProxygenHandlerFactory.h"

#ifdef HAVE_PROXYGEN

#include "ProxygenGetRequestHandler.h"
#include "ProxygenPutRequestHandler.h"
#include "ProxygenUnsupportedRequestHandler.h"

#include "Logger.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/lib/http/HTTPMethod.h>

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

    proxygen::RequestHandler* handler;
    if (message->getMethod() == proxygen::HTTPMethod::PUT) {
        handler = new ProxygenPutRequestHandler(m_web_app);
    }
    else if (message->getMethod() == proxygen::HTTPMethod::GET) {
        handler = new ProxygenGetRequestHandler(m_web_app);
    }
    else {
        handler = new ProxygenUnsupportedRequestHandler();
    }
    return handler;
}

}  // namespace hestia
#endif