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
    MockWebView(MockWebService* service) : WebView(), m_service(service)
    {
        m_can_stream = true;
    }

    HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user) override
    {
        m_user = user;

        auto buffer_size =
            m_service->get_data(request.get_context()->get_stream());
        auto response = HttpResponse::create();
        if (buffer_size == 0) {
            response->set_body("No data set!");
            return response;
        }
        return response;
    }

    HttpResponse::Ptr on_put(
        const HttpRequest& request, const User& user) override
    {
        m_user = user;

        LOG_INFO("Have headers: " << request.get_header().to_string());
        const auto content_length = request.get_header().get_content_length();

        if (request.get_context()->finished()) {
            return HttpResponse::create();
        }

        LOG_INFO(
            "Preparing internal persistent buffer of size " << content_length);
        m_service->set_data(
            std::stoi(content_length), request.get_context()->get_stream());

        if (request.is_content_outstanding()) {  // Fetch body if not complete
            return HttpResponse::create(
                HttpError(HttpError::Code::_100_CONTINUE));
        }

        auto status = request.get_context()->get_stream()->write(
            ReadableBufferView(request.body()));
        request.get_context()->on_input_complete();

        if (!status.ok() || request.get_context()->get_response()->error()) {
            LOG_ERROR(
                "Error writing body to stream: " << status.m_state.message());
            return HttpResponse::create(
                HttpError(HttpError::Code::_500_INTERNAL_SERVER_ERROR));
        }
        return HttpResponse::create();
    }

    hestia::User m_user;

  private:
    MockWebService* m_service{nullptr};
};
}  // namespace hestia::mock