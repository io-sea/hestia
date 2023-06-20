#include <catch2/catch_all.hpp>

#include "InMemoryKeyValueStoreClient.h"
#include "MockWebView.h"
#include "UrlRouter.h"
#include "UserService.h"
#include "WebApp.h"

class TestWebApp : public hestia::WebApp {
  public:
    TestWebApp(hestia::UserService* user_service) : hestia::WebApp(user_service)
    {
        m_url_router = std::make_unique<hestia::UrlRouter>();
        m_url_router->add_pattern(
            "/", std::make_unique<hestia::mock::MockWebView>(&m_service));
    }

    virtual ~TestWebApp() = default;

  private:
    hestia::mock::MockWebService m_service;
};

class TestWebAppTestFixture {
  public:
    TestWebAppTestFixture()
    {
        m_user_service = hestia::UserService::create({}, &m_kv_store_client);
        m_web_app      = std::make_unique<TestWebApp>(m_user_service.get());
    }

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    std::unique_ptr<TestWebApp> m_web_app;
    std::unique_ptr<hestia::UserService> m_user_service;
};


TEST_CASE_METHOD(TestWebAppTestFixture, "Test Basic web app", "[web_app]")
{
    hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
    hestia::RequestContext request_context(request);

    m_web_app->on_request(&request_context);

    auto response                      = request_context.get_response();
    const std::string no_data_response = "No data set!";

    REQUIRE(
        response->header().get_content_length()
        == std::to_string(no_data_response.size()));
    REQUIRE(response->body() == no_data_response);
}