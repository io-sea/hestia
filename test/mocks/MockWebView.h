#pragma once

#include "MockWebService.h"
#include "RequestContext.h"
#include "WebView.h"

#include "Logger.h"

namespace hestia::mock {
class MockWebView : public WebView {
  public:
    MockWebView(MockWebService* service) : WebView(), m_service(service) {}

    HttpResponse::Ptr on_get(const HttpRequest& request, const User&) override
    {
        std::string data;
        auto buffer_size =
            m_service->get_data(request.get_context()->get_stream());
        auto response = HttpResponse::create();
        if (buffer_size == 0) {
            response->set_body("No data set!");
        }
        return response;
    }

    HttpResponse::Ptr on_put(const HttpRequest& request, const User&) override
    {
        LOG_INFO("Have headers: " << request.get_header().to_string());
        const auto content_length = request.get_header().get_content_length();
        m_service->set_data(
            std::stoi(content_length), request.get_context()->get_stream());
        return HttpResponse::create();
    }

  private:
    MockWebService* m_service{nullptr};
};
}  // namespace hestia::mock