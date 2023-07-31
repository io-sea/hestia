#pragma once

#include "ProxygenRequestHandler.h"
#include "ReadableBufferView.h"

#ifdef HAVE_PROXYGEN

namespace hestia {
class ProxygenGetRequestHandler : public ProxygenRequestHandler {
  public:
    ProxygenGetRequestHandler(WebApp* web_app);

    void onRequest(
        std::unique_ptr<proxygen::HTTPMessage> msg) noexcept override;

    void onEOM() noexcept override;

  private:
    void on_output_chunk(
        const ReadableBufferView& buffer, bool finished, folly::EventBase* evb);

    void on_output_finished(const HttpResponse* response);
};
}  // namespace hestia
#endif