#pragma once

#include "ProxygenRequestHandler.h"
#include "ReadableBufferView.h"

#ifdef HAVE_PROXYGEN

namespace hestia {
class ProxygenPutRequestHandler : public ProxygenRequestHandler {
  public:
    ProxygenPutRequestHandler(WebApp* web_app);

    void onRequest(
        std::unique_ptr<proxygen::HTTPMessage> msg) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

  private:
    std::string m_body{};
};
}  // namespace hestia
#endif