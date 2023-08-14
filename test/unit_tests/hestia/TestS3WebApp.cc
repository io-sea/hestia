#include <catch2/catch_all.hpp>

#include "HestiaS3WebApp.h"

#include "ObjectStoreBackend.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "S3AuthorisationSession.h"
#include "StorageTier.h"
#include "TypedCrudRequest.h"
#include "UserService.h"

#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"
#include "RequestContext.h"

#include "Logger.h"
#include <iostream>

class MockS3TokenGenerator : public hestia::UserTokenGenerator {
  public:
    std::string generate(const std::string& key = {}) const override
    {
        (void)key;
        return m_token;
    }

    std::string m_token{"wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY"};
};

class WebAppTestFixture {
  public:
    WebAppTestFixture()
    {
        m_kv_store_client =
            std::make_unique<hestia::InMemoryKeyValueStoreClient>();

        m_object_store_client =
            std::make_unique<hestia::InMemoryHsmObjectStoreClient>();

        hestia::InMemoryObjectStoreClientConfig hsm_memory_client_config;
        hsm_memory_client_config.set_tiers({"0", "1", "2", "3", "4"});

        hestia::Dictionary serialized_config;
        hsm_memory_client_config.serialize(serialized_config);
        m_object_store_client->initialize("0", {}, serialized_config);

        hestia::ObjectStoreBackend object_store_backend(
            hestia::ObjectStoreBackend::Type::MEMORY_HSM);
        object_store_backend.set_tier_names({"0", "1", "2", "3", "4"});
        object_store_backend.set_config(serialized_config);

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());

        auto token_generator = std::make_unique<MockS3TokenGenerator>();
        m_token_generator    = token_generator.get();

        m_user_service = hestia::UserService::create(
            {}, &crud_backend, nullptr, nullptr, std::move(token_generator));
        auto register_response = m_user_service->register_user(
            "AKIAIOSFODNN7EXAMPLE", "my_admin_password");
        REQUIRE(register_response->ok());

        auto auth_response = m_user_service->authenticate_user(
            "AKIAIOSFODNN7EXAMPLE", "my_admin_password");
        REQUIRE(auth_response->ok());

        auto hsm_service = hestia::HsmService::create(
            hestia::ServiceConfig{}, m_kv_store_client.get(),
            m_object_store_client.get(), m_user_service.get(), nullptr);

        auto tier_service =
            hsm_service->get_service(hestia::HsmItem::Type::TIER);

        for (std::size_t idx = 0; idx < 5; idx++) {
            hestia::StorageTier tier(idx);
            auto response = tier_service->make_request(hestia::TypedCrudRequest{
                hestia::CrudMethod::CREATE,
                tier,
                {m_user_service->get_current_user().get_primary_key()}});
            REQUIRE(response->ok());
        }
        hsm_service->update_tiers(
            m_user_service->get_current_user().get_primary_key());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_is_server = true;
        dist_hsm_config.m_backends.push_back(object_store_backend);

        m_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), m_user_service.get());

        hestia::HestiaS3WebAppConfig app_config;
        m_web_app = std::make_unique<hestia::HestiaS3WebApp>(
            app_config, m_dist_hsm_service.get(), m_user_service.get());
    }

    void add_s3_headers(hestia::HttpRequest& req)
    {
        req.get_header().set_item("x-amz-date", "20130524T000000Z");

        hestia::S3AuthorisationObject auth_object;
        auth_object.m_date            = "20130524";
        auth_object.m_region          = "us-east-1";
        auth_object.m_user_identifier = "AKIAIOSFODNN7EXAMPLE";
        auth_object.m_user_key        = m_token_generator->m_token;
        auth_object.m_signed_headers  = {
            "host", "range", "x-amz-content-sha256", "x-amz-date"};

        auth_object.sort_headers();
        req.get_header().set_item(
            "Authorization", auth_object.get_credential_header(
                                 m_working_context->get_request()));
    }

    hestia::HttpResponse* make_request(
        const std::string& path, hestia::HttpRequest::Method method)
    {
        m_working_context = std::make_unique<hestia::RequestContext>();
        m_working_context->set_request(hestia::HttpRequest{path, method});
        add_s3_headers(m_working_context->get_writeable_request());

        m_web_app->on_event(
            m_working_context.get(), hestia::HttpEvent::HEADERS);
        if (m_working_context->get_response()->get_completion_status()
            != hestia::HttpResponse::CompletionStatus::FINISHED) {
            m_web_app->on_event(
                m_working_context.get(), hestia::HttpEvent::EOM);
        }
        return m_working_context->get_response();
    }

    hestia::HttpResponse* put_data(
        const std::string& path, const std::string& data)
    {
        hestia::HttpHeader header;
        header.set_item("Content-Length", std::to_string(data.size()));

        m_working_context = std::make_unique<hestia::RequestContext>();
        m_working_context->set_request(hestia::HttpRequest{
            path, hestia::HttpRequest::Method::PUT, header});
        add_s3_headers(m_working_context->get_writeable_request());

        m_web_app->on_event(
            m_working_context.get(), hestia::HttpEvent::HEADERS);

        if (m_working_context->get_response()->get_completion_status()
            != hestia::HttpResponse::CompletionStatus::FINISHED) {
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
                    data_chars.begin() + cursor,
                    data_chars.begin() + chunk_end);
                hestia::ReadableBufferView read_buffer(chunk);

                REQUIRE(m_working_context->write_to_stream(read_buffer).ok());
                cursor = chunk_end;
            }
            (void)m_working_context->clear_stream();

            m_web_app->on_event(
                m_working_context.get(), hestia::HttpEvent::EOM);
        }
        return m_working_context->get_response();
    }

    hestia::HttpResponse* get_data(const std::string& path, std::string& data)
    {
        m_working_context = std::make_unique<hestia::RequestContext>();
        m_working_context->set_request(
            hestia::HttpRequest(path, hestia::HttpRequest::Method::GET));
        add_s3_headers(m_working_context->get_writeable_request());

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

        m_web_app->on_event(
            m_working_context.get(), hestia::HttpEvent::HEADERS);
        if (m_working_context->get_response()->get_completion_status()
            != hestia::HttpResponse::CompletionStatus::FINISHED) {
            m_web_app->on_event(
                m_working_context.get(), hestia::HttpEvent::EOM);
        }

        auto response = m_working_context->get_response();
        REQUIRE(response->code() == 200);

        m_working_context->flush_stream();

        return response;
    }

    std::unique_ptr<hestia::InMemoryKeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::InMemoryHsmObjectStoreClient> m_object_store_client;

    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;

    MockS3TokenGenerator* m_token_generator{nullptr};
    std::unique_ptr<hestia::UserService> m_user_service;

    std::unique_ptr<hestia::HestiaS3WebApp> m_web_app;
    std::unique_ptr<hestia::RequestContext> m_working_context;
};

TEST_CASE_METHOD(WebAppTestFixture, "Test s3 web app", "[s3]")
{
    LOG_INFO("Starting test");
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
    response                   = put_data("/mybucket/myobject", obj_data);
    REQUIRE(response->code() == 200);

    // std::cout << m_kv_store_client->dump() << std::endl;

    std::string returned_data;
    response = get_data("/mybucket/myobject", returned_data);
    REQUIRE(response->code() == 200);
    REQUIRE(returned_data == obj_data);
}