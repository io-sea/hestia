#pragma once

#include "ReadableBufferView.h"
#include "RequestContext.h"
#include "WebApp.h"

#include <atomic>

#ifdef HAVE_PROXYGEN
#include <proxygen/httpserver/RequestHandler.h>

namespace hestia {
class ProxygenRequestHandler : public proxygen::RequestHandler {
  public:
    ProxygenRequestHandler(WebApp* web_app, std::size_t max_body_size);

    void onRequest(
        std::unique_ptr<proxygen::HTTPMessage> msg) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

    void requestComplete() noexcept override;

    void onError(proxygen::ProxygenError err) noexcept override;

    void onEgressPaused() noexcept override;

    void onEgressResumed() noexcept override;

    bool canHandleExpect() noexcept override { return true; }

  protected:
    void on_output_chunk(
        const ReadableBufferView& buffer, bool finished, folly::EventBase* evb);

    void on_output_finished(const HttpResponse*);

    std::unique_ptr<RequestContext> m_request_context;
    std::size_t m_max_body_size{0};
    std::atomic<bool> m_response_sent{false};
    WebApp* m_web_app{nullptr};
};
}  // namespace hestia
#endif