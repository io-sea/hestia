#include <catch2/catch_all.hpp>

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"

#include "DistributedHsmService.h"
#include "HttpObjectStoreClient.h"

#include "HsmObjectStoreClientBackend.h"
#include "HsmObjectStoreClientManager.h"
#include "HsmService.h"
#include "HttpClient.h"
#include "TypedCrudRequest.h"
#include "UserService.h"

#include "TestUtils.h"

#include <iostream>

class DistributedHsmServiceTestFixture {
  public:
    DistributedHsmServiceTestFixture()
    {
        m_kv_store_client =
            std::make_unique<hestia::InMemoryKeyValueStoreClient>();
        m_object_store_client =
            std::make_unique<hestia::InMemoryHsmObjectStoreClient>();

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());

        m_user_service = hestia::UserService::create({}, &crud_backend);
        auto register_response =
            m_user_service->register_user("my_admin", "my_admin_password");
        REQUIRE(register_response->ok());

        auto auth_response =
            m_user_service->authenticate_user("my_admin", "my_admin_password");
        REQUIRE(auth_response->ok());

        auto hsm_service = hestia::HsmService::create(
            hestia::ServiceConfig{}, m_kv_store_client.get(),
            m_object_store_client.get(), m_user_service.get());

        hestia::HsmObjectStoreClientBackend object_store_backend(
            hestia::HsmObjectStoreClientBackend::Type::MEMORY,
            hestia::InMemoryHsmObjectStoreClient::get_registry_identifier());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.add_backend(object_store_backend);

        m_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), &crud_backend,
            m_user_service.get());
    }

    std::unique_ptr<hestia::KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::InMemoryHsmObjectStoreClient> m_object_store_client;

    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;
    std::unique_ptr<hestia::UserService> m_user_service;
};

TEST_CASE_METHOD(
    DistributedHsmServiceTestFixture,
    "Distributed Hsm Service test",
    "[dist-hsm-service]")
{
    m_dist_hsm_service->register_self();

    hestia::CrudQuery query(hestia::CrudQuery::OutputFormat::ITEM);

    auto get_response = m_dist_hsm_service->get_node_service()->make_request(
        hestia::CrudRequest{query, {}});
    REQUIRE(get_response->ok());
    REQUIRE(get_response->items().size() == 1);

    std::string id("1234");
    hestia::HsmNode node(id);
    node.set_host_address("127.0.0.1");

    hestia::HsmObjectStoreClientBackend backend;
    backend.set_identifier("my_backend");
    node.add_backend(backend);

    auto put_response = m_dist_hsm_service->get_node_service()->make_request(
        hestia::TypedCrudRequest{hestia::CrudMethod::CREATE, node, {}});
    REQUIRE(put_response->ok());

    auto get_response2 = m_dist_hsm_service->get_node_service()->make_request(
        hestia::CrudRequest{query, {}});

    REQUIRE(get_response2->ok());
    REQUIRE(get_response2->items().size() == 2);
}
