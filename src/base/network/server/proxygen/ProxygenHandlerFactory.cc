#include "ProxygenHandlerFactory.h"

#ifdef HAVE_PROXYGEN

#include "ProxygenRequestHandler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/lib/http/HTTPMethod.h>

namespace hestia {

ProxygenHandlerFactory::ProxygenHandlerFactory(
    WebApp* web_app, std::size_t max_buffer_size) :
    m_web_app(web_app), m_max_buffer_size(max_buffer_size)
{
}

void ProxygenHandlerFactory::onServerStart(folly::EventBase*) noexcept {}

void ProxygenHandlerFactory::onServerStop() noexcept {}

proxygen::RequestHandler* ProxygenHandlerFactory::onRequest(
    proxygen::RequestHandler*, proxygen::HTTPMessage*) noexcept
{
    return new ProxygenRequestHandler(m_web_app, m_max_buffer_size);
}

}  // namespace hestia
#endif