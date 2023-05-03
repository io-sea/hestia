#include <catch2/catch_all.hpp>

#include "BasicHttpServer.h"
#include "MockWebView.h"
#include "UrlRouter.h"

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

TEST_CASE("Test Basic Http Server", "[server]")
{
    TestWebApp web_app;

    hestia::Server::Config test_config;
    hestia::BasicHttpServer server(test_config, &web_app);

    server.initialize();
    // server.start();
}