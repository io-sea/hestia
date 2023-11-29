#include <catch2/catch_all.hpp>

#include "DistributedHsmObjectStoreClient.h"

#include "DistributedHsmService.h"

#include "ApplicationMiddleware.h"
#include "HestiaHsmActionView.h"
#include "PingView.h"
#include "RequestContext.h"
#include "UrlRouter.h"
#include "WebApp.h"

#include "HsmObjectStoreClientManager.h"
#include "HsmService.h"
#include "HttpClient.h"
#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "TypedCrudRequest.h"
#include "UserService.h"

#include "TestUtils.h"

#include <future>
#include <iostream>

class MockHsmMiddleware : public hestia::ApplicationMiddleware {
  public:
    hestia::HttpResponse::Ptr call(
        const hestia::HttpRequest& request,
        hestia::AuthorizationContext& user,
        hestia::HttpEvent,
        hestia::responseProviderFunc func)
    {
        user = m_test_user;
        return func(request);
    }

    hestia::AuthorizationContext m_test_user;
};

class MockHsmWebApp : public hestia::WebApp {
  public:
    MockHsmWebApp(hestia::DistributedHsmService* hsm_service) :
        WebApp(hsm_service->get_user_service())
    {
        const std::string api_prefix = "/api/v1/";

        m_url_router = std::make_unique<hestia::UrlRouter>();

        m_url_router->add_pattern(
            {api_prefix + hestia::HsmItem::hsm_action_name + "s"},
            std::make_unique<hestia::HestiaHsmActionView>(hsm_service));

        m_url_router->add_pattern(
            {api_prefix + "ping"}, std::make_unique<hestia::PingView>());

        auto middleware   = std::make_unique<MockHsmMiddleware>();
        auto current_user = hsm_service->get_user_service()->get_current_user();

        middleware->m_test_user.m_user_id    = current_user.get_primary_key();
        middleware->m_test_user.m_user_token = current_user.tokens()[0].value();
        add_middleware(std::move(middleware));
    }
};

class MockHsmHttpClient : public hestia::HttpClient {
  public:
    void add_app(
        hestia::DistributedHsmService* hsm_service, const std::string& address)
    {
        m_apps[address] = std::make_unique<MockHsmWebApp>(hsm_service);
    }

    void make_request(
        const hestia::HttpRequest& request,
        hestia::HttpClient::completionFunc completion_func,
        hestia::Stream* stream,
        std::size_t,
        progressFunc) override
    {
        LOG_INFO("Got request to: " << request.get_path());

        const auto stripped_path =
            hestia::StringUtils::remove_prefix(request.get_path(), "http://");

        std::string working_address;
        MockHsmWebApp* working_app{nullptr};
        for (const auto& [address, app] : m_apps) {
            if (hestia::StringUtils::starts_with(stripped_path, address)) {
                working_address = address;
                working_app     = app.get();
            }
        }

        if (working_app == nullptr) {
            completion_func(hestia::HttpResponse::create(404, "Not Found"));
        }

        auto intercepted_request = request;
        intercepted_request.overwrite_path(
            hestia::StringUtils::remove_prefix(stripped_path, working_address));

        hestia::RequestContext request_context;
        request_context.set_request(intercepted_request);

        working_app->on_event(&request_context, hestia::HttpEvent::HEADERS);
        if (request_context.get_response()->code() == 307) {
            working_address = hestia::StringUtils::remove_prefix(
                request_context.get_response()->header().get_item("location"),
                "http://");
            const auto& [start, rest] =
                hestia::StringUtils::split_on_first(working_address, "/");
            working_address = start;

            LOG_INFO("Got redirect to: " + working_address);
            auto app_iter = m_apps.find(working_address);
            working_app   = nullptr;
            if (app_iter != m_apps.end()) {
                working_app = app_iter->second.get();
            }
            if (working_app == nullptr) {
                completion_func(hestia::HttpResponse::create(404, "Not Found"));
                return;
            }
            working_app->on_event(&request_context, hestia::HttpEvent::HEADERS);
        }

        working_app->on_event(&request_context, hestia::HttpEvent::EOM);

        LOG_INFO("Has stream - proceeding? " << bool(stream));

        if (stream != nullptr) {
            LOG_INFO("Has stream - doing PUT");
            if (request.get_method() == hestia::HttpRequest::Method::PUT) {
                std::vector<char> buffer(1024);
                hestia::WriteableBufferView buffer_view(buffer);
                auto read_result = stream->read(buffer_view);
                if (read_result.m_num_transferred > 0) {
                    auto write_result = request_context.get_stream()->write(
                        hestia::ReadableBufferView(
                            &buffer[0], read_result.m_num_transferred));
                    LOG_INFO(
                        "Wrote: " << write_result.m_num_transferred
                                  << " bytes.");
                    REQUIRE(write_result.ok());
                    LOG_INFO("About to reset stream");
                    REQUIRE(request_context.get_stream()->reset().ok());
                }
            }
            else if (request.get_method() == hestia::HttpRequest::Method::GET) {
                std::vector<char> buffer(1024);
                hestia::WriteableBufferView buffer_view(buffer);
                auto read_result =
                    request_context.get_stream()->read(buffer_view);
                LOG_INFO(
                    "Read: " << read_result.m_num_transferred << " bytes: "
                             << std::string(buffer.begin(), buffer.end()));
                if (read_result.m_num_transferred > 0) {
                    auto write_result =
                        stream->write(hestia::ReadableBufferView(
                            buffer.data(), read_result.m_num_transferred));
                    LOG_INFO(
                        "Wrote: " << write_result.m_num_transferred
                                  << " bytes.");
                    REQUIRE(write_result.ok());
                }
            }
        }
        auto response = std::make_unique<hestia::HttpResponse>(
            *request_context.get_response());
        LOG_INFO("Setting location header to: " << working_address);
        response->header().set_item("location", working_address);
        completion_func(std::move(response));
    }

    std::unordered_map<std::string, std::unique_ptr<MockHsmWebApp>> m_apps;
};

class DistributedHsmObjectStoreClientTestFixture {
  public:
    DistributedHsmObjectStoreClientTestFixture()
    {
        m_kv_store_client =
            std::make_unique<hestia::InMemoryKeyValueStoreClient>();

        setup_controller();
        setup_worker();
        setup_local_client();
    }

    void setup_local_client()
    {
        LOG_INFO("Setting up client");
        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(nullptr);
        m_local_object_store_client =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_http_client.get());

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());

        auto hsm_child_services =
            std::make_unique<hestia::HsmServiceCollection>();
        hsm_child_services->create_default_services(
            {}, &crud_backend, m_user_service.get());

        hsm_child_services->get_service(hestia::HsmItem::Type::DATASET)
            ->set_default_name("test_default_dataset");

        auto hsm_service = std::make_unique<hestia::HsmService>(
            hestia::ServiceConfig{}, std::move(hsm_child_services),
            m_local_object_store_client.get());
        hsm_service->update_tiers(m_test_user.get_primary_key());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_controller_address = "45678";
        m_local_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), m_user_service.get());

        m_local_object_store_client->do_initialize(
            {}, m_local_dist_hsm_service.get());
    }

    void setup_controller()
    {
        LOG_INFO("Setting up controller");
        m_http_client = std::make_unique<MockHsmHttpClient>();

        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(nullptr);

        m_controller_object_store_client =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_http_client.get());

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());

        m_user_service = hestia::UserService::create({}, &crud_backend);
        auto register_response =
            m_user_service->register_user("my_admin", "my_admin_password");
        REQUIRE(register_response->ok());

        auto auth_response =
            m_user_service->authenticate_user("my_admin", "my_admin_password");
        REQUIRE(auth_response->ok());
        m_test_user = *auth_response->get_item_as<hestia::User>();

        auto hsm_child_services =
            std::make_unique<hestia::HsmServiceCollection>();
        hsm_child_services->create_default_services(
            {}, &crud_backend, m_user_service.get());

        hsm_child_services->get_service(hestia::HsmItem::Type::DATASET)
            ->set_default_name("test_default_dataset");

        auto tier_service =
            hsm_child_services->get_service(hestia::HsmItem::Type::TIER);

        for (std::size_t idx = 0; idx < 5; idx++) {
            hestia::StorageTier tier(std::to_string(idx));
            tier.set_priority(idx);
            auto response = tier_service->make_request(hestia::TypedCrudRequest{
                hestia::CrudMethod::CREATE,
                tier,
                {},
                {m_test_user.get_primary_key()}});
            REQUIRE(response->ok());
        }

        auto hsm_service = std::make_unique<hestia::HsmService>(
            hestia::ServiceConfig{}, std::move(hsm_child_services),
            m_controller_object_store_client.get());
        hsm_service->update_tiers(m_test_user.get_primary_key());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.set_is_controller(true);
        dist_hsm_config.m_self.set_host_address("45678");
        dist_hsm_config.m_self.set_name("my_controller");
        dist_hsm_config.m_is_server = true;

        m_controller_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), m_user_service.get());

        m_controller_dist_hsm_service->register_self();

        m_http_client->add_app(m_controller_dist_hsm_service.get(), "45678");

        m_controller_object_store_client->do_initialize(
            {}, m_controller_dist_hsm_service.get());
    }

    void setup_worker()
    {
        LOG_INFO("Setting up worker");
        auto client_factory =
            std::make_unique<hestia::HsmObjectStoreClientFactory>(nullptr);
        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(
                std::move(client_factory));

        m_worker_object_store_client =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_http_client.get());

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());

        auto hsm_child_services =
            std::make_unique<hestia::HsmServiceCollection>();
        hsm_child_services->create_default_services(
            {}, &crud_backend, m_user_service.get());

        hsm_child_services->get_service(hestia::HsmItem::Type::DATASET)
            ->set_default_name("test_default_dataset");

        auto hsm_service = std::make_unique<hestia::HsmService>(
            hestia::ServiceConfig{}, std::move(hsm_child_services),
            m_worker_object_store_client.get());
        hsm_service->update_tiers(m_test_user.get_primary_key());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.set_is_controller(false);
        dist_hsm_config.m_self.set_host_address("12345");
        dist_hsm_config.m_self.set_name("my_worker");
        dist_hsm_config.m_is_server = true;

        hestia::InMemoryObjectStoreClientConfig hsm_memory_client_config;
        hestia::Dictionary serialized_config;
        hsm_memory_client_config.serialize(serialized_config);

        hestia::ObjectStoreBackend object_store_backend(
            hestia::ObjectStoreBackend::Type::MEMORY_HSM);
        object_store_backend.set_tier_ids({"0", "1", "2", "3", "4"});
        object_store_backend.set_config(serialized_config);

        dist_hsm_config.m_backends = {object_store_backend};

        m_worker_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), m_user_service.get());

        m_worker_dist_hsm_service->register_self();

        m_http_client->add_app(m_worker_dist_hsm_service.get(), "12345");

        m_worker_object_store_client->do_initialize(
            {}, m_worker_dist_hsm_service.get());
    }

    void create_object(const std::string& id)
    {
        auto create_response =
            m_local_dist_hsm_service->get_hsm_service()->make_request(
                hestia::CrudRequest{
                    hestia::CrudMethod::CREATE,
                    {hestia::CrudIdentifier(id)},
                    {m_test_user.get_primary_key()}},
                hestia::HsmItem::hsm_object_name);
    }

    void put_data(
        const hestia::StorageObject& obj,
        const std::string& content,
        uint8_t tier)
    {
        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::PUT_DATA);
        action.set_subject_key(obj.get_primary_key());
        action.set_target_tier(tier);

        std::promise<hestia::HsmActionResponse::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::HsmActionResponse::Ptr response) {
                response_promise.set_value(std::move(response));
            };

        hestia::Stream stream;
        stream.set_source(hestia::InMemoryStreamSource::create(
            hestia::ReadableBufferView{content}));
        m_local_dist_hsm_service->do_hsm_action(
            hestia::HsmActionRequest(action, {m_test_user.get_primary_key()}),
            &stream, completion_cb);

        if (stream.waiting_for_content()) {
            LOG_INFO("Going to flush stream");
            REQUIRE(stream.flush().ok());
        }

        const auto response = response_future.get();
        REQUIRE(response->ok());
    }

    void get_data(
        const hestia::StorageObject& obj,
        std::string& content,
        std::size_t content_length,
        uint8_t tier)
    {
        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::GET_DATA);
        action.set_subject_key(obj.get_primary_key());
        action.set_source_tier(tier);

        std::promise<hestia::HsmActionResponse::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::HsmActionResponse::Ptr response) {
                response_promise.set_value(std::move(response));
            };

        hestia::Stream stream;
        std::vector<char> return_buffer(content_length);
        hestia::WriteableBufferView writeable_buffer(return_buffer);
        stream.set_sink(hestia::InMemoryStreamSink::create(writeable_buffer));

        m_local_dist_hsm_service->do_hsm_action(
            hestia::HsmActionRequest(action, {m_test_user.get_primary_key()}),
            &stream, completion_cb);

        if (stream.has_content()) {
            LOG_INFO("Going to flush stream");
            REQUIRE(stream.flush().ok());
        }

        const auto response = response_future.get();
        REQUIRE(response->ok());

        content = std::string(return_buffer.begin(), return_buffer.end());
    }

    std::unique_ptr<hestia::InMemoryKeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<MockHsmHttpClient> m_http_client;

    std::unique_ptr<hestia::DistributedHsmObjectStoreClient>
        m_local_object_store_client;
    std::unique_ptr<hestia::DistributedHsmObjectStoreClient>
        m_worker_object_store_client;
    std::unique_ptr<hestia::DistributedHsmObjectStoreClient>
        m_controller_object_store_client;

    std::unique_ptr<hestia::DistributedHsmService> m_local_dist_hsm_service;
    std::unique_ptr<hestia::DistributedHsmService> m_worker_dist_hsm_service;
    std::unique_ptr<hestia::DistributedHsmService>
        m_controller_dist_hsm_service;

    std::unique_ptr<hestia::UserService> m_user_service;
    hestia::User m_test_user;
};

TEST_CASE_METHOD(
    DistributedHsmObjectStoreClientTestFixture,
    "Test Distributed Hsm Object Store Client",
    "[hsm]")
{
    std::string id{"0000"};
    create_object(id);

    hestia::StorageObject obj(id);
    obj.set_metadata("mykey", "myval");

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.size());

    put_data(obj, content, 0);

    std::string tier0_content;
    hestia::StorageObject obj0(id);
    get_data(obj0, tier0_content, content.length(), 0);
    REQUIRE(tier0_content == content);
}