#include <catch2/catch_all.hpp>

#include "MockWebView.h"
#include "UrlRouter.h"
#include "WebApp.h"

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

TEST_CASE("Test Basic web app", "[web_app]")
{
    TestWebApp web_app;

    hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
    hestia::RequestContext request_context(request);
    return;

    web_app.on_request(&request_context);

    auto response                      = request_context.get_response();
    const std::string no_data_response = "No data set!";

    REQUIRE(
        response->header().get_content_length()
        == std::to_string(no_data_response.size()));
    REQUIRE(response->body() == no_data_response);
}