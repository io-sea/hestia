#pragma once

#include "ReadableBufferView.h"
#include "WebApp.h"

#ifdef HAVE_PROXYGEN
#include <proxygen/httpserver/RequestHandler.h>

namespace hestia {
class ProxygenRequestHandler : public proxygen::RequestHandler {
  public:
    ProxygenRequestHandler(WebApp* web_app);

    void onRequest(
        std::unique_ptr<proxygen::HTTPMessage> msg) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

    void requestComplete() noexcept override;

    void onError(proxygen::ProxygenError err) noexcept override;

    void onEgressPaused() noexcept override;

    void onEgressResumed() noexcept override;

  private:
    void on_output_chunk(
        const ReadableBufferView& buffer, bool finished, folly::EventBase* evb);

    void on_output_finished(const HttpResponse* response);

    std::unique_ptr<RequestContext> m_request_context;
    WebApp* m_web_app{nullptr};
};
}  // namespace hestia
#endif