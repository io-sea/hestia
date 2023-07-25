#pragma once

#include "ProxygenRequestHandler.h"

#ifdef HAVE_PROXYGEN

namespace hestia {
class ProxygenUnsupportedRequestHandler : public ProxygenRequestHandler {
  public:
    ProxygenUnsupportedRequestHandler();

    void onRequest(
        std::unique_ptr<proxygen::HTTPMessage> msg) noexcept override;

    void onEOM() noexcept override;
};
}  // namespace hestia
#endif