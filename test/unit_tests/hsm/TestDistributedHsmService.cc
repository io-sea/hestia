#include <catch2/catch_all.hpp>

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "FileKeyValueStoreClient.h"

#include "CopyToolInterface.h"
#include "DistributedHsmService.h"
#include "HsmNodeService.h"
#include "HsmObjectStoreClientManager.h"
#include "HsmObjectStoreClientSpec.h"
#include "HsmService.h"
#include "MultiBackendHsmObjectStoreClient.h"
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
            hestia::MultiBackendHsmObjectStoreClient::create();

        hestia::HsmObjectStoreClientSpec object_store_backend(
            hestia::HsmObjectStoreClientSpec::Type::HSM,
            hestia::HsmObjectStoreClientSpec::Source::BUILT_IN,
            "hestia::FileHsmObjectStoreClient");
        object_store_backend.m_extra_config.set_item("root", get_store_path());

        hestia::TierBackendRegistry backends;
        backends.emplace(0, object_store_backend);
        backends.emplace(1, object_store_backend);
        m_object_store_client->do_initialize(backends, {});

        m_hsm_service = hestia::HsmService::create(
            m_kv_store_client.get(), m_object_store_client.get());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        m_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, m_hsm_service.get(), m_kv_store_client.get());
    }

    std::string get_store_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    std::string m_test_name;
    std::unique_ptr<hestia::KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::MultiBackendHsmObjectStoreClient>
        m_object_store_client;
    std::unique_ptr<hestia::HsmService> m_hsm_service;
    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;
};

TEST_CASE_METHOD(
    DistributedHsmServiceTestFixture,
    "Distributed Hsm Service test",
    "[dist-hsm-service]")
{
    init("TestDistributedHsmService");

    std::vector<hestia::HsmNode> nodes;
    m_dist_hsm_service->get(nodes);

    REQUIRE(nodes.size() == 1);

    hestia::HsmNode node("01234");
    node.m_host_address = "127.0.0.1";

    hestia::ObjectStoreBackend backend;
    backend.m_identifier = "my_backend";
    node.m_backends.push_back(backend);

    m_dist_hsm_service->put(node);

    m_dist_hsm_service->get(nodes);

    REQUIRE(nodes.size() == 2);
    REQUIRE(nodes[1].m_host_address == node.m_host_address);
    REQUIRE(nodes[1].m_backends.size() == 1);
}
