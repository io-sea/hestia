#include <catch2/catch_all.hpp>

#include "HestiaS3WebApp.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "MockHttpClient.h"

#include <future>
#include <iostream>

class WebAppTestFixture {
  public:
    WebAppTestFixture()
    {
        m_client = std::make_unique<hestia::mock::MockHttpClientForS3>(
            "127.0.0.1:8000");
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
    std::unique_ptr<hestia::mock::MockHttpClientForS3> m_client;
};

TEST_CASE_METHOD(WebAppTestFixture, "Test s3 web app", "[s3]")
{
    auto response = make_request(
        hestia::HttpRequest("/", hestia::HttpRequest::Method::GET));

    REQUIRE(response->code() == 200);
    REQUIRE_FALSE(response->body().empty());
    REQUIRE(
        response->header().get_content_length()
        == std::to_string(response->body().size()));


    response = make_request(
        hestia::HttpRequest("/mybucket", hestia::HttpRequest::Method::GET));
    REQUIRE(response->code() == 404);

    response = make_request(
        hestia::HttpRequest("/mybucket", hestia::HttpRequest::Method::PUT));
    REQUIRE(response->code() == 201);

    response = make_request(
        hestia::HttpRequest("/mybucket", hestia::HttpRequest::Method::GET));
    REQUIRE(response->code() == 200);
    REQUIRE_FALSE(response->body().empty());
    REQUIRE(
        response->header().get_content_length()
        == std::to_string(response->body().size()));

    response = make_request(hestia::HttpRequest(
        "/mybucket/myobject", hestia::HttpRequest::Method::GET));
    REQUIRE(response->code() == 404);

    std::string obj_data = "The quick brown fox jumps over the lazy dog.";
    hestia::ReadableBufferView read_buffer(obj_data);

    hestia::Stream stream;
    stream.set_source(hestia::InMemoryStreamSource::create(read_buffer));

    response = make_request(
        hestia::HttpRequest(
            "/mybucket/myobject", hestia::HttpRequest::Method::PUT),
        &stream);
    REQUIRE(response->code() == 200);
    (void)stream.reset();

    response = make_request(hestia::HttpRequest(
        "/mybucket/myobject", hestia::HttpRequest::Method::HEAD));
    REQUIRE(response->code() == 200);

    std::vector<char> returned_data(obj_data.size());
    hestia::WriteableBufferView write_buffer(returned_data);
    stream.set_sink(hestia::InMemoryStreamSink::create(write_buffer));

    response = make_request(
        hestia::HttpRequest(
            "/mybucket/myobject", hestia::HttpRequest::Method::GET),
        &stream);
    REQUIRE(response->code() == 200);

    std::string returned_str(returned_data.begin(), returned_data.end());
    REQUIRE(returned_str == obj_data);
}