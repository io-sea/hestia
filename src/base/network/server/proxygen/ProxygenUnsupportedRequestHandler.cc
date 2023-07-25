#include "ProxygenUnsupportedRequestHandler.h"

#ifdef HAVE_PROXYGEN

#include "HttpResponse.h"
#include "ProxygenMessage.h"

#include "Logger.h"

namespace hestia {
ProxygenUnsupportedRequestHandler::ProxygenUnsupportedRequestHandler() :
    ProxygenRequestHandler(nullptr)
{
}

void ProxygenUnsupportedRequestHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> msg) noexcept
{
    LOG_WARN("Method not supported: " << msg->getMethodString());
    auto response =
        HttpResponse::create(HttpError(HttpError::Code::_404_NOT_FOUND));
    response->set_body("Method not supported: " + msg->getMethodString());
    ProxygenMessage::build_response(downstream_, response.get(), false);
}

void ProxygenUnsupportedRequestHandler::onEOM() noexcept
{
    ProxygenMessage::build_response(downstream_, nullptr, true);
}
}  // namespace hestia
#endif