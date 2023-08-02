#include <catch2/catch_all.hpp>

#include "DistributedHsmObjectStoreClient.h"

#include "DistributedHsmService.h"

#include "DataPlacementEngine.h"
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
#include "TypedCrudRequest.h"
#include "UserService.h"

#include "TestUtils.h"

#include <iostream>

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
    }
};

class MockHsmHttpClient : public hestia::HttpClient {
  public:
    void add_app(
        hestia::DistributedHsmService* hsm_service, const std::string& address)
    {
        m_apps[address] = std::make_unique<MockHsmWebApp>(hsm_service);
    }

    hestia::HttpResponse::Ptr make_request(
        const hestia::HttpRequest& request, hestia::Stream*) override
    {
        LOG_INFO("Got request to: " << request.get_path());

        std::string working_address;
        MockHsmWebApp* working_app{nullptr};
        for (const auto& [address, app] : m_apps) {
            if (hestia::StringUtils::starts_with(request.get_path(), address)) {
                working_address = address;
                working_app     = app.get();
            }
        }

        if (working_app == nullptr) {
            return hestia::HttpResponse::create(404, "Not Found");
        }

        auto intercepted_request = request;
        intercepted_request.overwrite_path(hestia::StringUtils::remove_prefix(
            request.get_path(), working_address));

        hestia::RequestContext request_context(intercepted_request);

        working_app->on_request(&request_context);

        if (request_context.get_response()->code() == 307) {
            working_address =
                request_context.get_response()->header().get_item("location");
            LOG_INFO("Got redirect to: " + working_address);
            auto app_iter = m_apps.find(working_address);
            working_app   = nullptr;
            if (app_iter != m_apps.end()) {
                working_app = app_iter->second.get();
            }
            if (working_app == nullptr) {
                return hestia::HttpResponse::create(404, "Not Found");
            }
            working_app->on_request(&request_context);
        }
        return std::make_unique<hestia::HttpResponse>(
            *request_context.get_response());
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
            hestia::StorageTier tier(idx);
            auto response = tier_service->make_request(hestia::TypedCrudRequest{
                hestia::CrudMethod::CREATE,
                tier,
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

        hestia::ObjectStoreBackend object_store_backend(
            hestia::ObjectStoreBackend::Type::MEMORY_HSM);
        object_store_backend.set_tier_names({"0", "1", "2", "3", "4"});

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
                    {m_test_user.get_primary_key()},
                    {hestia::CrudIdentifier(id)}},
                hestia::HsmItem::hsm_object_name);
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
    std::cout << m_kv_store_client->dump() << std::endl;

    std::string id{"0000"};
    create_object(id);
    return;

    hestia::StorageObject obj(id);
    obj.set_metadata("mykey", "myval");

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.size());

    hestia::Stream stream;

    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::PUT);
    REQUIRE(m_local_object_store_client->make_request(request, &stream)->ok());

    REQUIRE(stream.write(content).ok());
    REQUIRE(stream.reset().ok());
}