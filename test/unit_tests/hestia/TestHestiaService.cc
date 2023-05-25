#include <catch2/catch_all.hpp>

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "FileKeyValueStoreClient.h"

#include "CopyToolInterface.h"
#include "HestiaNodeService.h"
#include "HestiaService.h"
#include "HsmObjectStoreClientManager.h"
#include "HsmObjectStoreClientSpec.h"
#include "HsmService.h"
#include "MultiBackendHsmObjectStoreClient.h"
#include "ObjectService.h"
#include "TierService.h"

#include "TestUtils.h"

#include <iostream>

class HestiaServiceTestFixture {
  public:
    ~HestiaServiceTestFixture()
    {
        // std::filesystem::remove_all(get_store_path());
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

        hestia::ObjectServiceConfig object_config;
        auto object_service = std::make_unique<hestia::ObjectService>(
            object_config, m_kv_store_client.get());

        auto tier_service = std::make_unique<hestia::TierService>(
            hestia::TierServiceConfig(), m_kv_store_client.get());

        auto client_factory =
            std::make_unique<hestia::HsmObjectStoreClientFactory>(nullptr);
        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(
                std::move(client_factory));
        m_object_store_client =
            std::make_unique<hestia::MultiBackendHsmObjectStoreClient>(
                std::move(client_manager));

        hestia::HsmObjectStoreClientSpec my_spec(
            hestia::HsmObjectStoreClientSpec::Type::HSM,
            hestia::HsmObjectStoreClientSpec::Source::BUILT_IN,
            "hestia::FileHsmObjectStoreClient");
        my_spec.m_extra_config.set_item("root", get_store_path());
        hestia::TierBackendRegistry g_all_file_backend_example;
        g_all_file_backend_example.emplace(0, my_spec);
        g_all_file_backend_example.emplace(1, my_spec);

        m_object_store_client->do_initialize(g_all_file_backend_example, {});
        auto placement_engine =
            std::make_unique<hestia::BasicDataPlacementEngine>(
                tier_service.get());

        m_hsm_service = std::make_unique<hestia::HsmService>(
            std::move(object_service), std::move(tier_service),
            m_object_store_client.get(), std::move(placement_engine));

        hestia::HestiaNodeServiceConfig node_config;
        auto node_service = std::make_unique<hestia::HestiaNodeService>(
            node_config, m_kv_store_client.get());

        hestia::HestiaServiceConfig hestia_config;
        m_hestia_service = std::make_unique<hestia::HestiaService>(
            hestia_config, m_hsm_service.get(), std::move(node_service));
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
    std::unique_ptr<hestia::HestiaService> m_hestia_service;
};

TEST_CASE_METHOD(
    HestiaServiceTestFixture, "Hestia Service test", "[hestia-service]")
{
    init("TestHestiaService");

    std::vector<hestia::HestiaNode> nodes;
    m_hestia_service->get(nodes);

    REQUIRE(nodes.empty());

    hestia::HestiaNode node("01234");
    node.m_host_address = "127.0.0.1";

    m_hestia_service->put(node);

    m_hestia_service->get(nodes);

    REQUIRE(nodes.size() == 1);
    REQUIRE(nodes[0].m_host_address == node.m_host_address);
}
