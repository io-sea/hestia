#include <catch2/catch_all.hpp>

#include "BasicHttpServer.h"
#include "CurlClient.h"
#include "MockWebView.h"
#include "UrlRouter.h"

#include <iostream>

class TestWebApp : public hestia::WebApp {
  public:
    TestWebApp() : hestia::WebApp()
    {
        m_url_router = std::make_unique<hestia::UrlRouter>();
        m_url_router->add_pattern(
            "/", std::make_unique<hestia::mock::MockWebView>(&m_service));
    }

  private:
    hestia::mock::MockWebService m_service;
};

TEST_CASE("Test Curl client", "[curl]")
{
    return;
    TestWebApp web_app;

    hestia::Server::Config test_config;
    hestia::BasicHttpServer server(test_config, &web_app);

    server.initialize();
    server.start();
    server.wait_until_bound();

    hestia::CurlClientConfig config;

    hestia::CurlClient client(config);

    const auto url = "127.0.0.1:8000/";
    hestia::HttpRequest request(url, hestia::HttpRequest::Method::GET);

    auto response = client.make_request(request);
    std::cerr << response->body() << std::endl;
}