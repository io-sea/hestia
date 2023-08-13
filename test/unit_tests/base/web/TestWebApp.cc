#include <catch2/catch_all.hpp>

#include "InMemoryKeyValueStoreClient.h"
#include "MockWebView.h"
#include "TokenAuthenticationMiddleware.h"
#include "UrlRouter.h"
#include "UserService.h"
#include "WebApp.h"

class TestWebApp : public hestia::WebApp {
  public:
    TestWebApp(hestia::UserService* user_service) : hestia::WebApp(user_service)
    {
        m_url_router = std::make_unique<hestia::UrlRouter>();

        auto view = std::make_unique<hestia::mock::MockWebView>(&m_service);
        m_view    = view.get();

        m_url_router->add_pattern({"/"}, std::move(view));
    }

    virtual ~TestWebApp() = default;

    hestia::mock::MockWebView* m_view{nullptr};

  private:
    hestia::mock::MockWebService m_service;
};

class TestWebAppTestFixture {
  public:
    TestWebAppTestFixture()
    {
        hestia::KeyValueStoreCrudServiceBackend backend(&m_kv_store_client);
        m_user_service = hestia::UserService::create({}, &backend);

        auto register_response =
            m_user_service->register_user("my_admin", "my_admin_password");
        REQUIRE(register_response->ok());
        auto user             = register_response->get_item_as<hestia::User>();
        m_test_user.m_user_id = user->get_primary_key();
        m_test_user.m_user_token = user->get_first_token().value();
        m_web_app = std::make_unique<TestWebApp>(m_user_service.get());
    }

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    std::unique_ptr<TestWebApp> m_web_app;
    std::unique_ptr<hestia::UserService> m_user_service;
    hestia::AuthorizationContext m_test_user;
};


TEST_CASE_METHOD(TestWebAppTestFixture, "Test Basic web app", "[web_app]")
{
    hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
    hestia::RequestContext request_context;
    request_context.set_request(request);

    m_web_app->on_event(&request_context, hestia::HttpEvent::HEADERS);

    auto response                      = request_context.get_response();
    const std::string no_data_response = "No data set!";

    REQUIRE(
        response->header().get_content_length()
        == std::to_string(no_data_response.size()));
    REQUIRE(response->body() == no_data_response);
}

TEST_CASE_METHOD(
    TestWebAppTestFixture, "Test Token auth middleware", "[web_app]")
{
    hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
    request.get_header().set_auth_token(m_test_user.m_user_token);

    hestia::RequestContext request_context;
    request_context.set_request(request);

    auto token_auth = std::make_unique<hestia::TokenAuthenticationMiddleware>();
    token_auth->set_user_service(m_user_service.get());

    m_web_app->add_middleware(std::move(token_auth));

    m_web_app->on_event(&request_context, hestia::HttpEvent::HEADERS);

    auto response                      = request_context.get_response();
    const std::string no_data_response = "No data set!";

    REQUIRE(
        response->header().get_content_length()
        == std::to_string(no_data_response.size()));
    REQUIRE(response->body() == no_data_response);

    REQUIRE(m_web_app->m_view->m_user.m_user_id == m_test_user.m_user_id);
}