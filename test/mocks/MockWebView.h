#pragma once

#include "HttpResponse.h"
#include "MockWebService.h"
#include "RequestContext.h"
#include "WebView.h"

#include "InMemoryStreamSource.h"
#include "ReadableBufferView.h"
#include "WriteableBufferView.h"

#include "Logger.h"

namespace hestia::mock {
class MockWebView : public WebView {
  public:
    MockWebView(MockWebService* service, bool should_redirect = true) :
        WebView(), m_service(service)
    {
        m_should_redirect = should_redirect;
    }

    HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override
    {
        m_user = auth;

        auto redirect_url = request.get_header().get_item("redirect_me");
        if (m_should_redirect && !redirect_url.empty()) {
            auto response = HttpResponse::create(307, "Found");
            LOG_INFO("Returning redirect");
            response->header().set_item("location", "http://" + redirect_url);
            return response;
        }

        if (event == HttpEvent::HEADERS) {
            auto buffer_size =
                m_service->get_data(request.get_context()->get_stream());

            if (buffer_size == 0) {
                auto response = HttpResponse::create();
                response->set_body("No data set!");
                return response;
            }
            else {
                LOG_INFO("Returning await eom");
                return HttpResponse::create(
                    HttpResponse::CompletionStatus::AWAITING_EOM);
            }
        }
        else {
            return HttpResponse::create();
        }
    }

    HttpResponse::Ptr on_put(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override
    {
        m_user = auth;

        auto redirect_url = request.get_header().get_item("redirect_me");
        if (m_should_redirect && !redirect_url.empty()) {
            auto response = HttpResponse::create(302, "Found");
            LOG_INFO("Returning redirect");
            response->header().set_item("location", "http://" + redirect_url);
            return response;
        }

        if (event == HttpEvent::HEADERS) {
            const auto content_length =
                request.get_header().get_content_length();
            LOG_INFO("Preparing internal buffer of size " << content_length);
            m_service->set_data(
                std::stoi(content_length), request.get_context()->get_stream());
            LOG_INFO("Completion status is awaiting body chunk");
            return HttpResponse::create(
                HttpResponse::CompletionStatus::AWAITING_BODY_CHUNK);
        }
        else {
            LOG_INFO("Handling EOM");
            return HttpResponse::create();
        }
    }

    hestia::AuthorizationContext m_user;

  private:
    bool m_should_redirect{true};
    MockWebService* m_service{nullptr};
};
}  // namespace hestia::mock