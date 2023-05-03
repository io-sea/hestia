#include <string>

#ifdef HAVE_PROXYGEN
#include <catch2/catch_all.hpp>

#include "ProxygenServer.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "Logger.h"
#include "RequestContext.h"
#include "UrlRouter.h"

class TestWebService {
  public:
    std::size_t get_data(hestia::Stream* stream)
    {
        if (m_buffer.empty()) {
            return 0;
        }
        stream->set_source(hestia::InMemoryStreamSource::create(m_buffer));
        return m_buffer.size();
    }

    std::size_t get_data(std::string& val)
    {
        val = std::string(m_buffer.begin(), m_buffer.end());
        return m_buffer.size();
    }

    void set_data(std::size_t size, hestia::Stream* stream)
    {
        m_buffer.clear();
        m_buffer.resize(size);
        stream->set_sink(hestia::InMemoryStreamSink::create(m_buffer));
    }

  private:
    std::vector<char> m_buffer;
};

class TestWebView : public hestia::WebView {
  public:
    TestWebView(TestWebService* service) : hestia::WebView(), m_service(service)
    {
    }

    hestia::HttpResponse::Ptr on_get(
        const hestia::HttpRequest& request) override
    {
        // auto buffer_size =
        // mService->getData(request.getContext()->getStream());
        // LOG_INFO("Buffer size is: " + std::to_string(buffer_size));

        std::string data;
        auto buffer_size = m_service->get_data(data);
        auto response    = hestia::HttpResponse::create();
        if (buffer_size == 0) {
            response->set_body("No data set!");
        }
        else {
            response->header().set_item(
                "Content-Length", std::to_string(buffer_size));
            response->set_body(data);
        }
        return response;
    }

    hestia::HttpResponse::Ptr on_put(
        const hestia::HttpRequest& request) override
    {
        const auto content_length = request.get_header().get_content_length();
        m_service->set_data(
            std::stoi(content_length), request.get_context()->get_stream());

        return hestia::HttpResponse::create();
    }

  private:
    TestWebService* m_service{nullptr};
};

class TestWebApp : public hestia::WebApp {
  public:
    TestWebApp() : hestia::WebApp()
    {
        m_url_router = std::make_unique<hestia::UrlRouter>();
        m_url_router->add_pattern(
            "/", std::make_unique<TestWebView>(&m_service));
    }

  private:
    TestWebService m_service;
};

TEST_CASE("Test Proxygen Server", "[proxygen]")
{
    TestWebApp web_app;

    hestia::Server::Config test_config;
    hestia::ProxygenServer server(test_config, &web_app);

    server.initialize();
    // server.start();
}
#endif