#include <catch2/catch_all.hpp>

#include "BasicHttpServer.h"
#include "MockWebView.h"
#include "UrlRouter.h"

#include "InMemoryKeyValueStoreClient.h"
#include "UserService.h"

class TestWebApp : public hestia::WebApp {
  public:
    TestWebApp(hestia::UserService* user_service) : hestia::WebApp(user_service)
    {
        m_url_router = std::make_unique<hestia::UrlRouter>();
        m_url_router->add_pattern(
            "/", std::make_unique<hestia::mock::MockWebView>(&m_service));
    }

  private:
    hestia::mock::MockWebService m_service;
};

class TestBasicHttpServerFixture {
  public:
    TestBasicHttpServerFixture()
    {
        m_user_service = hestia::UserService::create({}, &m_kv_store_client);
        m_web_app      = std::make_unique<TestWebApp>(m_user_service.get());
    }

    std::unique_ptr<TestWebApp> m_web_app;
    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    std::unique_ptr<hestia::UserService> m_user_service;
};

TEST_CASE_METHOD(
    TestBasicHttpServerFixture, "Test Basic Http Server", "[server]")
{
    hestia::Server::Config test_config;
    hestia::BasicHttpServer server(test_config, m_web_app.get());

    server.initialize();
    // server.start();
}