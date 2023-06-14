#include <catch2/catch_all.hpp>

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "FileKeyValueStoreClient.h"

#include "DistributedHsmObjectStoreClient.h"
#include "DistributedHsmService.h"
#include "HsmNodeService.h"
#include "HsmObjectStoreClientBackend.h"
#include "HsmObjectStoreClientManager.h"
#include "HsmService.h"
#include "ObjectService.h"
#include "TierService.h"

#include "TestUtils.h"

#include <iostream>

class DistributedHsmServiceTestFixture {
  public:
    ~DistributedHsmServiceTestFixture()
    {
        std::filesystem::remove_all(get_store_path());
    }

    void init(const std::string& test_name)
    {
        m_test_name           = test_name;
        const auto store_path = get_store_path();
        std::filesystem::remove_all(store_path);
        hestia::Metadata config;
        config.set_item("root", store_path);

        m_kv_store_client = std::make_unique<hestia::FileKeyValueStoreClient>();
        m_kv_store_client->initialize(config);

        m_object_store_client =
            hestia::DistributedHsmObjectStoreClient::create();

        auto hsm_service = hestia::HsmService::create(
            m_kv_store_client.get(), m_object_store_client.get());

        hestia::HsmObjectStoreClientBackend object_store_backend(
            hestia::HsmObjectStoreClientBackend::Type::HSM,
            hestia::HsmObjectStoreClientBackend::Source::BUILT_IN,
            "hestia::FileHsmObjectStoreClient");
        object_store_backend.m_extra_config.set_item("root", get_store_path());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.m_backends = {object_store_backend};

        m_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), m_kv_store_client.get());

        m_object_store_client->do_initialize(m_dist_hsm_service.get());
    }

    std::string get_store_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    std::string m_test_name;
    std::unique_ptr<hestia::KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::DistributedHsmObjectStoreClient>
        m_object_store_client;
    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;
};

TEST_CASE_METHOD(
    DistributedHsmServiceTestFixture,
    "Distributed Hsm Service test",
    "[dist-hsm-service]")
{
    init("TestDistributedHsmService");

    auto get_response = m_dist_hsm_service->make_request(
        hestia::DistributedHsmServiceRequestMethod::GET);
    REQUIRE(get_response->ok());
    REQUIRE(get_response->items().size() == 1);

    hestia::HsmNode node("01234");
    node.m_host_address = "127.0.0.1";

    hestia::HsmObjectStoreClientBackend backend;
    backend.m_identifier = "my_backend";
    node.m_backends.push_back(backend);

    auto put_response = m_dist_hsm_service->make_request(
        {node, hestia::DistributedHsmServiceRequestMethod::PUT});
    REQUIRE(put_response->ok());

    auto get_response2 = m_dist_hsm_service->make_request(
        hestia::DistributedHsmServiceRequestMethod::GET);
    REQUIRE(get_response2->ok());

    REQUIRE(get_response2->items().size() == 2);
    REQUIRE(get_response2->items()[1].m_host_address == node.m_host_address);
    REQUIRE(get_response2->items()[1].m_backends.size() == 1);
}
