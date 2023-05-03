#pragma once

#include "WebApp.h"

#ifdef HAVE_PROXYGEN
#include <proxygen/httpserver/RequestHandlerFactory.h>

namespace hestia {

class ProxygenHandlerFactory : public proxygen::RequestHandlerFactory {
  public:
    ProxygenHandlerFactory(WebApp* web_app);

    void onServerStart(folly::EventBase*) noexcept override;

    void onServerStop() noexcept override;

    proxygen::RequestHandler* onRequest(
        proxygen::RequestHandler*,
        proxygen::HTTPMessage* message) noexcept override;

  private:
    WebApp* m_web_app{nullptr};
};
}  // namespace hestia

#endif
