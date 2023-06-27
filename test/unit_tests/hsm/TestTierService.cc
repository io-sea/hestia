#include <catch2/catch_all.hpp>

#include "FileKeyValueStoreClient.h"
#include "StorageTier.h"
#include "StringAdapter.h"

#include "TestUtils.h"
#include "TierService.h"

#include <filesystem>

class TierServiceTestFixture {
  public:
    void init(const std::string& test_name)
    {
        m_test_name           = test_name;
        const auto store_path = get_store_path();
        std::filesystem::remove_all(store_path);

        hestia::Metadata config;
        config.set_item("root", store_path);
        m_kv_store_client = std::make_unique<hestia::FileKeyValueStoreClient>();
        m_kv_store_client->initialize(config);

        hestia::TierServiceConfig tier_config;
        m_service =
            hestia::TierService::create(tier_config, m_kv_store_client.get());
    }

    ~TierServiceTestFixture()
    {
        // std::filesystem::remove_all(get_store_path());
    }

    void put(const hestia::StorageTier& tier)
    {
        hestia::CrudRequest<hestia::StorageTier> req(
            tier, hestia::CrudMethod::PUT);
        auto response = m_service->make_request(req);
        REQUIRE(response->ok());
    }

    void get(hestia::StorageTier& tier)
    {
        auto response =
            m_service->make_request({tier, hestia::CrudMethod::GET});
        REQUIRE(response->ok());
        tier = response->item();
    }

    bool exists(const hestia::StorageTier& tier)
    {
        auto response =
            m_service->make_request({tier, hestia::CrudMethod::EXISTS});
        REQUIRE(response->ok());
        return response->found();
    }

    void list(std::vector<hestia::Uuid>& ids)
    {
        auto response = m_service->make_request({hestia::CrudMethod::LIST});
        REQUIRE(response->ok());
        ids = response->ids();
    }

    void remove(const hestia::StorageTier& tier)
    {
        auto response =
            m_service->make_request({tier, hestia::CrudMethod::REMOVE});
        REQUIRE(response->ok());
    }

    std::string get_store_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    std::string m_test_name;
    std::unique_ptr<hestia::KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::TierService> m_service;
};

TEST_CASE_METHOD(
    TierServiceTestFixture, "Test Tier Service - Get and Set", "[tier_service]")
{
    init("TestTierService");

    hestia::StorageTier tier0("0");
    tier0.m_backend = "phobos";
    hestia::StorageTier tier1("1");
    hestia::StorageTier tier2("2");

    put(tier0);
    put(tier1);

    hestia::StorageTier retrieved_tier0("0");
    get(retrieved_tier0);
    REQUIRE(retrieved_tier0.m_backend == tier0.m_backend);

    REQUIRE(exists(retrieved_tier0));

    hestia::StorageTier retrieved_tier1("1");
    get(retrieved_tier1);
    REQUIRE(exists(retrieved_tier1));

    std::vector<hestia::Uuid> ids;
    list(ids);
    REQUIRE(ids.size() == 2);
    REQUIRE(
        ((ids[0] == retrieved_tier0.id()) || (ids[1] == retrieved_tier0.id())));
    REQUIRE(
        ((ids[0] == retrieved_tier1.id()) || (ids[1] == retrieved_tier1.id())));

    remove(retrieved_tier0);

    REQUIRE_FALSE(exists(retrieved_tier0));
    std::vector<hestia::Uuid> updated_ids;
    list(updated_ids);
    REQUIRE(updated_ids.size() == 1);
}