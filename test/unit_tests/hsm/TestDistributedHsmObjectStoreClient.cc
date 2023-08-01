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
    void init(hestia::DistributedHsmService* m_hsm_service)
    {
        m_app = std::make_unique<MockHsmWebApp>(m_hsm_service);
    }

    hestia::HttpResponse::Ptr make_request(
        const hestia::HttpRequest& request, hestia::Stream*) override
    {
        auto intercepted_request = request;
        intercepted_request.overwrite_path(
            hestia::StringUtils::remove_prefix(request.get_path(), m_address));

        hestia::RequestContext request_context(intercepted_request);
        m_app->on_request(&request_context);
        return std::make_unique<hestia::HttpResponse>(
            *request_context.get_response());
    }

    std::string m_address{"127.0.0.1"};
    std::unique_ptr<MockHsmWebApp> m_app;
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
        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(nullptr);
        m_local_object_store_client =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_controller_http_client.get());

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
        m_local_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), &crud_backend,
            m_user_service.get());

        m_local_object_store_client->do_initialize(
            {}, m_local_dist_hsm_service.get());
    }

    void setup_controller()
    {
        m_controller_http_client = std::make_unique<MockHsmHttpClient>();

        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(nullptr);

        m_controller_object_store_client =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_controller_http_client.get());

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
            tier.set_backend(hestia::InMemoryHsmObjectStoreClient::
                                 get_registry_identifier());
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

        m_controller_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), &crud_backend,
            m_user_service.get());

        m_controller_dist_hsm_service->register_self();

        m_controller_http_client->init(m_controller_dist_hsm_service.get());

        m_controller_object_store_client->do_initialize(
            {}, m_controller_dist_hsm_service.get());
    }

    void setup_worker()
    {
        m_worker_http_client = std::make_unique<MockHsmHttpClient>();

        auto client_factory =
            std::make_unique<hestia::HsmObjectStoreClientFactory>(nullptr);
        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(
                std::move(client_factory));

        m_worker_object_store_client =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_worker_http_client.get());

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

        hestia::HsmObjectStoreClientBackend object_store_backend(
            hestia::HsmObjectStoreClientBackend::Type::MEMORY_HSM,
            hestia::InMemoryHsmObjectStoreClient::get_registry_identifier());
        dist_hsm_config.m_self.set_backends({object_store_backend});

        m_worker_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), &crud_backend,
            m_user_service.get());

        m_worker_dist_hsm_service->register_self();

        m_worker_http_client->init(m_worker_dist_hsm_service.get());

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

    std::unique_ptr<hestia::KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<MockHsmHttpClient> m_controller_http_client;
    std::unique_ptr<MockHsmHttpClient> m_worker_http_client;

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
    return;

    hestia::Stream stream;

    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::PUT);
    REQUIRE(m_local_object_store_client->make_request(request, &stream)->ok());

    REQUIRE(stream.write(content).ok());
    REQUIRE(stream.reset().ok());
}