#pragma once

#include "MockWebService.h"
#include "RequestContext.h"
#include "WebView.h"

#include "InMemoryStreamSource.h"
#include "ReadableBufferView.h"

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
        if (!request.get_context()
                 ->get_stream()
                 ->has_content()) {  // If no stream look at body
            auto source = InMemoryStreamSource::create(
                ReadableBufferView(request.body()));
            request.get_context()->get_stream()->set_source(std::move(source));
        }
        m_service->set_data(
            std::stoi(content_length), request.get_context()->get_stream());
        auto status = request.get_context()->get_stream()->flush();
        if (!status.ok()) {
            LOG_ERROR("Error saving request data");
        }
        return HttpResponse::create();
    }

  private:
    MockWebService* m_service{nullptr};
};
}  // namespace hestia::mock