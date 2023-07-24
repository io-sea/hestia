#ifdef HAVE_PROXYGEN
#include <catch2/catch_all.hpp>

#include "ProxygenServer.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "Logger.h"
#include "RequestContext.h"
#include "UrlRouter.h"

#include "MockWebService.h"
#include "MockWebView.h"
#include "UrlRouter.h"

#include "InMemoryKeyValueStoreClient.h"
#include "UserService.h"

#include <memory>

class TestWebApp : public hestia::WebApp {
  public:
    TestWebApp(hestia::UserService* user_service) : hestia::WebApp(user_service)
    {
        m_url_router = std::make_unique<hestia::UrlRouter>();
        m_url_router->add_pattern(
            {"/"}, std::make_unique<hestia::mock::MockWebView>(&m_service));
    }

  private:
    hestia::mock::MockWebService m_service;
};


class TestProxygenServerFixture {
  public:
    TestProxygenServerFixture()
    {
        hestia::KeyValueStoreCrudServiceBackend backend(&m_kv_store_client);
        m_user_service = hestia::UserService::create({}, &backend);
        m_web_app      = std::make_unique<TestWebApp>(m_user_service.get());
    }

    std::unique_ptr<TestWebApp> m_web_app;
    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    std::unique_ptr<hestia::UserService> m_user_service;
};

TEST_CASE_METHOD(
    TestProxygenServerFixture, "Test Proxygen Server", "[proxygen]")
{
    hestia::Server::Config test_config;
    hestia::ProxygenServer server(test_config, m_web_app.get());

    server.initialize();
    server.start();
    server.wait_until_bound();

    // Other testing done with curl client

    server.stop();
}
#endif