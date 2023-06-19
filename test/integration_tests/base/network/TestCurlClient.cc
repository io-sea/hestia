#include <catch2/catch_all.hpp>

#include "BasicHttpServer.h"
#include "CurlClient.h"
#include "MockWebView.h"
#include "UrlRouter.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

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

TEST_CASE("Test Curl client - Default", "[curl]")
{
    TestWebApp web_app;

    hestia::Server::Config test_config;
    hestia::BasicHttpServer server(test_config, &web_app);

    server.initialize();
    server.start();
    server.wait_until_bound();

    hestia::CurlClientConfig config;

    hestia::CurlClient client(config);

    const auto url = "127.0.0.1:8000/";
    hestia::HttpRequest get_request(url, hestia::HttpRequest::Method::GET);

    auto get_response = client.make_request(get_request);
    REQUIRE(get_response->body() == "No data set!");

    hestia::HttpRequest put_request(url, hestia::HttpRequest::Method::PUT);
    put_request.body() = "The quick brown fox jumps over the lazy dog.";

    auto put_response = client.make_request(put_request);

    auto get_response1 = client.make_request(get_request);
    REQUIRE(
        get_response1->body()
        == "The quick brown fox jumps over the lazy dog.");
}

TEST_CASE("Test Curl client - Streams", "[curl]")
{
    TestWebApp web_app;

    hestia::Server::Config test_config;
    hestia::BasicHttpServer server(test_config, &web_app);

    server.initialize();
    server.start();
    server.wait_until_bound();

    hestia::CurlClientConfig config;

    hestia::CurlClient client(config);

    std::string content = "The quick brown fox jumps over the lazy dog.";

    const auto url = "127.0.0.1:8000/";
    hestia::HttpRequest put_request(url, hestia::HttpRequest::Method::PUT);

    hestia::Stream stream;
    auto source = std::make_unique<hestia::InMemoryStreamSource>(
        hestia::ReadableBufferView(content));
    stream.set_source(std::move(source));

    auto put_response = client.make_request(put_request, &stream);
    REQUIRE(!put_response->error());

    REQUIRE(stream.reset().ok());

    std::vector<char> returned_content(content.size());
    hestia::WriteableBufferView writeable_buffer(returned_content);

    auto sink = std::make_unique<hestia::InMemoryStreamSink>(writeable_buffer);
    stream.set_sink(std::move(sink));

    hestia::HttpRequest get_request(url, hestia::HttpRequest::Method::GET);
    auto get_response = client.make_request(get_request, &stream);
    REQUIRE(!get_response->error());

    std::string reconstructed_response(
        returned_content.begin(), returned_content.end());
    REQUIRE(reconstructed_response == content);
}
