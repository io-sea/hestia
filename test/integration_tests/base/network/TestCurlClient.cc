#include <catch2/catch_all.hpp>

#include "CurlClient.h"
#include "MockWebView.h"
#include "UrlRouter.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "InMemoryKeyValueStoreClient.h"
#include "UserService.h"

#include "BasicHttpServer.h"
#include "File.h"
#include "TestUtils.h"

#include <future>
#include <iostream>
#include <memory>

class TestWebApp : public hestia::WebApp {
  public:
    TestWebApp(hestia::UserService* user_service, bool should_redirect = true) :
        hestia::WebApp(user_service)
    {
        m_url_router = std::make_unique<hestia::UrlRouter>();
        m_url_router->add_pattern(
            {"/"}, std::make_unique<hestia::mock::MockWebView>(
                       &m_service, should_redirect));
    }

  private:
    hestia::mock::MockWebService m_service;
};

class TestCurlClientFixture {
  public:
    TestCurlClientFixture()
    {
        hestia::KeyValueStoreCrudServiceBackend backend(&m_kv_store_client);
        m_user_service = hestia::UserService::create({}, &backend);
        m_web_app      = std::make_unique<TestWebApp>(m_user_service.get());

        hestia::BaseServerConfig test_config;
        m_server = std::make_unique<hestia::BasicHttpServer>(
            test_config, m_web_app.get());

        m_server->initialize();
        m_server->start();
        m_server->wait_until_bound();

        hestia::CurlClientConfig config;
        m_client = std::make_unique<hestia::CurlClient>(config);
    }

    void run_redirect_server()
    {
        m_redirect_web_app =
            std::make_unique<TestWebApp>(m_user_service.get(), false);

        hestia::BaseServerConfig test_config;
        test_config.m_http_port = 8090;
        m_redirect_server       = std::make_unique<hestia::BasicHttpServer>(
            test_config, m_redirect_web_app.get());

        m_redirect_server->initialize();
        m_redirect_server->start();
        m_redirect_server->wait_until_bound();
    }

    hestia::HttpResponse::Ptr make_request(
        const hestia::HttpRequest& request, hestia::Stream* stream = nullptr)
    {
        std::promise<hestia::HttpResponse::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::HttpResponse::Ptr response) {
                response_promise.set_value(std::move(response));
            };
        m_client->make_request(request, completion_cb, stream);
        return response_future.get();
    }

    ~TestCurlClientFixture() { m_server->stop(); }

    const std::string m_url          = "127.0.0.1:8000/";
    const std::string m_redirect_url = "127.0.0.1:8090/";

    std::unique_ptr<hestia::CurlClient> m_client;

    std::unique_ptr<hestia::BasicHttpServer> m_server;
    std::unique_ptr<TestWebApp> m_web_app;

    std::unique_ptr<hestia::BasicHttpServer> m_redirect_server;
    std::unique_ptr<TestWebApp> m_redirect_web_app;

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    std::unique_ptr<hestia::UserService> m_user_service;
};

TEST_CASE_METHOD(TestCurlClientFixture, "Test Curl client - Default", "[curl]")
{
    hestia::HttpRequest get_request(m_url, hestia::HttpRequest::Method::GET);

    auto get_response = make_request(get_request);
    REQUIRE(get_response->body() == "No data set!");

    hestia::HttpRequest put_request(m_url, hestia::HttpRequest::Method::PUT);
    put_request.body() = "The quick brown fox jumps over the lazy dog.";

    auto put_response = make_request(put_request);

    auto get_response1 = make_request(get_request);
    REQUIRE(
        get_response1->body()
        == "The quick brown fox jumps over the lazy dog.");
}

TEST_CASE_METHOD(TestCurlClientFixture, "Test Curl client - Streams", "[curl]")
{
    std::string content = "The quick brown fox jumps over the lazy dog.";

    hestia::HttpRequest put_request(m_url, hestia::HttpRequest::Method::PUT);

    hestia::Stream stream;
    auto source = std::make_unique<hestia::InMemoryStreamSource>(
        hestia::ReadableBufferView(content));
    stream.set_source(std::move(source));

    auto put_response = make_request(put_request, &stream);
    REQUIRE(!put_response->error());

    REQUIRE(stream.reset().ok());

    std::vector<char> returned_content(content.size());
    hestia::WriteableBufferView writeable_buffer(returned_content);

    auto sink = std::make_unique<hestia::InMemoryStreamSink>(writeable_buffer);
    stream.set_sink(std::move(sink));

    hestia::HttpRequest get_request(m_url, hestia::HttpRequest::Method::GET);
    auto get_response = make_request(get_request, &stream);
    REQUIRE(!get_response->error());

    std::string reconstructed_response(
        returned_content.begin(), returned_content.end());
    REQUIRE(reconstructed_response == content);
}

TEST_CASE_METHOD(
    TestCurlClientFixture, "Test Curl client - Large Body", "[curl]")
{
    hestia::File file(TestUtils::get_test_data_dir() / "EmperorWu.txt");
    std::string content;
    file.read(content);

    hestia::HttpRequest put_request(m_url, hestia::HttpRequest::Method::PUT);

    hestia::Stream stream;
    auto source = std::make_unique<hestia::InMemoryStreamSource>(
        hestia::ReadableBufferView(content));
    stream.set_source(std::move(source));

    auto put_response = make_request(put_request, &stream);
    REQUIRE(!put_response->error());

    REQUIRE(stream.reset().ok());

    std::vector<char> returned_content(content.size());
    hestia::WriteableBufferView writeable_buffer(returned_content);

    auto sink = std::make_unique<hestia::InMemoryStreamSink>(writeable_buffer);
    stream.set_sink(std::move(sink));

    hestia::HttpRequest get_request(m_url, hestia::HttpRequest::Method::GET);
    auto get_response = make_request(get_request, &stream);
    REQUIRE(!get_response->error());

    std::string reconstructed_response(
        returned_content.begin(), returned_content.end());
    REQUIRE(reconstructed_response == content);
}

TEST_CASE_METHOD(TestCurlClientFixture, "Test Redirect", "[.curl]")
{
    run_redirect_server();

    hestia::HttpRequest get_request(m_url, hestia::HttpRequest::Method::GET);
    get_request.get_header().set_item("redirect_me", m_redirect_url);

    auto get_response = make_request(get_request);
    REQUIRE(get_response->body() == "No data set!");

    hestia::HttpRequest put_request(m_url, hestia::HttpRequest::Method::PUT);
    put_request.get_header().set_item("redirect_me", m_redirect_url);
    put_request.body() = "The quick brown fox jumps over the lazy dog.";
    auto put_response  = make_request(put_request);

    auto get_response1 = make_request(get_request);
    REQUIRE(
        get_response1->body()
        == "The quick brown fox jumps over the lazy dog.");
}

TEST_CASE_METHOD(
    TestCurlClientFixture, "Test Curl client - Large Body Redirect", "[.curl]")
{
    run_redirect_server();

    hestia::File file(TestUtils::get_test_data_dir() / "EmperorWu.txt");
    std::string content;
    file.read(content);

    hestia::HttpRequest put_request(m_url, hestia::HttpRequest::Method::PUT);
    put_request.get_header().set_item("redirect_me", m_redirect_url);

    hestia::Stream stream;
    auto source = std::make_unique<hestia::InMemoryStreamSource>(
        hestia::ReadableBufferView(content));
    stream.set_source(std::move(source));

    auto put_response = make_request(put_request, &stream);
    REQUIRE(!put_response->error());

    REQUIRE(stream.reset().ok());

    std::vector<char> returned_content(content.size());
    hestia::WriteableBufferView writeable_buffer(returned_content);

    auto sink = std::make_unique<hestia::InMemoryStreamSink>(writeable_buffer);
    stream.set_sink(std::move(sink));

    hestia::HttpRequest get_request(m_url, hestia::HttpRequest::Method::GET);
    get_request.get_header().set_item("redirect_me", m_redirect_url);

    auto get_response = make_request(get_request, &stream);
    REQUIRE(!get_response->error());

    std::string reconstructed_response(
        returned_content.begin(), returned_content.end());
    REQUIRE(reconstructed_response == content);
}
