#include <catch2/catch_all.hpp>

#include "S3Service.h"
// #include "S3WebApp.h"
#include "UserService.h"

#include "InMemoryKeyValueStoreClient.h"
#include "InMemoryObjectStoreClient.h"
#include "RequestContext.h"

#include <iostream>

#ifdef __HESTIA_DUMMY__

class WebAppTestFixture {
  public:
    WebAppTestFixture()
    {
        m_object_store = hestia::InMemoryObjectStoreClient::create();

        m_user_service = hestia::UserService::create({}, &m_kv_store_client);

        hestia::S3ServiceConfig service_config;
        service_config.m_object_store_client = m_object_store.get();
        auto s3_service = std::make_unique<hestia::S3Service>(service_config);

        hestia::S3WebAppConfig config;

        m_web_app = std::make_unique<hestia::S3WebApp>(
            m_object_store.get(), m_user_service.get(), config,
            std::move(s3_service));
    }

    hestia::HttpResponse* make_request(
        const std::string& path, hestia::HttpRequest::Method method)
    {
        hestia::HttpRequest request(path, method);
        m_working_context = std::make_unique<hestia::RequestContext>(request);
        m_web_app->on_request(m_working_context.get());
        return m_working_context->get_response();
    }

    hestia::HttpResponse* put_data(
        const std::string& path, const std::string& data)
    {
        hestia::HttpHeader header;
        header.set_item("Content-Length", std::to_string(data.size()));

        hestia::HttpRequest request(
            path, hestia::HttpRequest::Method::PUT, header);
        m_working_context = std::make_unique<hestia::RequestContext>(request);
        m_web_app->on_request(m_working_context.get());

        auto response = m_working_context->get_response();
        REQUIRE(response->code() == 201);

        std::size_t chunk_size = 10;
        std::vector<char> data_chars(data.begin(), data.end());
        std::size_t cursor = 0;
        while (cursor < data.size()) {
            auto chunk_end = cursor + chunk_size;
            if (chunk_end >= data.size()) {
                chunk_end = data.size();
            }
            if (chunk_end == cursor) {
                break;
            }

            std::vector<char> chunk(
                data_chars.begin() + cursor, data_chars.begin() + chunk_end);
            hestia::ReadableBufferView read_buffer(chunk);

            REQUIRE(m_working_context->write_to_stream(read_buffer).ok());
            cursor = chunk_end;
        }
        m_working_context->on_input_complete();
        return m_working_context->get_response();
    }

    hestia::HttpResponse* get_data(const std::string& path, std::string& data)
    {
        hestia::HttpRequest request(path, hestia::HttpRequest::Method::GET);
        m_working_context = std::make_unique<hestia::RequestContext>(request);

        std::vector<char> working_buffer;
        m_working_context->set_output_chunk_handler(
            [&working_buffer](
                const hestia::ReadableBufferView& buffer, bool finished) {
                (void)finished;
                for (std::size_t idx = 0; idx < buffer.length(); idx++) {
                    working_buffer.push_back(buffer.data()[idx]);
                }
                return buffer.length();
            });

        m_working_context->set_output_complete_handler(
            [&working_buffer, &data](const hestia::HttpResponse* response) {
                REQUIRE(response->code() == 200);
                data =
                    std::string(working_buffer.begin(), working_buffer.end());
            });

        m_web_app->on_request(m_working_context.get());

        auto response = m_working_context->get_response();
        REQUIRE(response->code() == 200);

        m_working_context->flush_stream();

        return response;
    }

    std::unique_ptr<hestia::InMemoryObjectStoreClient> m_object_store;
    hestia::InMemoryKeyValueStoreClient m_kv_store_client;

    std::unique_ptr<hestia::UserService> m_user_service;
    std::unique_ptr<hestia::S3WebApp> m_web_app;
    std::unique_ptr<hestia::RequestContext> m_working_context;
};

TEST_CASE_METHOD(WebAppTestFixture, "Test s3 web app", "[s3]")
{
    auto response = make_request("/", hestia::HttpRequest::Method::GET);
    REQUIRE(response->code() == 200);
    REQUIRE_FALSE(response->body().empty());
    REQUIRE(
        response->header().get_content_length()
        == std::to_string(response->body().size()));

    response = make_request("/mybucket", hestia::HttpRequest::Method::GET);
    REQUIRE(response->code() == 404);

    response = make_request("/mybucket", hestia::HttpRequest::Method::PUT);
    REQUIRE(response->code() == 201);

    response = make_request("/mybucket", hestia::HttpRequest::Method::GET);
    REQUIRE(response->code() == 200);
    REQUIRE_FALSE(response->body().empty());
    REQUIRE(
        response->header().get_content_length()
        == std::to_string(response->body().size()));

    response =
        make_request("/mybucket/myobject", hestia::HttpRequest::Method::GET);
    REQUIRE(response->code() == 404);

    response =
        make_request("/mybucket/myobject", hestia::HttpRequest::Method::PUT);
    REQUIRE(response->code() == 201);

    response =
        make_request("/mybucket/myobject", hestia::HttpRequest::Method::HEAD);
    REQUIRE(response->code() == 200);

    const std::string obj_data = "The quick brown fox jumps over the lazy dog.";

    response = put_data("/mybucket/myobject", obj_data);
    REQUIRE(response->code() == 201);

    std::string returned_data;
    response = get_data("/mybucket/myobject", returned_data);
    REQUIRE(response->code() == 200);
    REQUIRE(returned_data == obj_data);
}

#endif