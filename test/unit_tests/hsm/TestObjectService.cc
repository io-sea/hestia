#include <catch2/catch_all.hpp>

#include "FileKeyValueStoreClient.h"
#include "HsmObjectAdapter.h"
#include "ObjectService.h"
#include "TestUtils.h"

#include <filesystem>

class ObjectServiceTestFixture {
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

        hestia::ObjectServiceConfig object_config;
        object_config.m_global_prefix = "hestia";
        m_object_service              = hestia::ObjectService::create(
            object_config, m_kv_store_client.get());
    }

    ~ObjectServiceTestFixture()
    {
        std::filesystem::remove_all(get_store_path());
    }

    void put(const hestia::HsmObject& obj)
    {
        auto response =
            m_object_service->make_request({obj, hestia::CrudMethod::PUT});
        REQUIRE(response->ok());
    }

    void get(hestia::HsmObject& obj)
    {
        auto response =
            m_object_service->make_request({obj, hestia::CrudMethod::GET});
        REQUIRE(response->ok());
        obj = response->item();
    }

    bool exists(const hestia::HsmObject& obj)
    {
        auto response =
            m_object_service->make_request({obj, hestia::CrudMethod::EXISTS});
        REQUIRE(response->ok());
        return response->found();
    }

    void list(std::vector<std::string>& ids)
    {
        auto response =
            m_object_service->make_request({hestia::CrudMethod::LIST});
        REQUIRE(response->ok());
        ids = response->ids();
    }

    void remove(const hestia::HsmObject& obj)
    {
        auto response =
            m_object_service->make_request({obj, hestia::CrudMethod::REMOVE});
        REQUIRE(response->ok());
    }

    std::string get_store_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    std::string m_test_name;
    std::unique_ptr<hestia::KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::ObjectService> m_object_service;
};

TEST_CASE_METHOD(
    ObjectServiceTestFixture,
    "Test Object Service - Get and Set",
    "[object_service]")
{
    init("TestObjectService");

    hestia::HsmObject object0(1234);
    object0.object().m_metadata.set_item("key0", "value0");
    object0.object().m_metadata.set_item("key1", "value1");
    object0.object().m_metadata.set_item("key2", "value2");

    object0.add_tier(1);
    object0.add_tier(2);

    hestia::HsmObject object1(56789);
    hestia::HsmObject object2(4321);

    put(object0);
    put(object1);

    REQUIRE(exists(object0));
    REQUIRE(exists(object1));
    REQUIRE_FALSE(exists(object2));

    hestia::HsmObject retrieved_object0(1234);
    get(retrieved_object0);
    REQUIRE(retrieved_object0.object().m_metadata.get_item("key0") == "value0");
    REQUIRE(retrieved_object0.object().m_metadata.get_item("key1") == "value1");
    REQUIRE(retrieved_object0.object().m_metadata.get_item("key2") == "value2");

    REQUIRE(retrieved_object0.tiers().size() == 2);

    std::vector<std::string> ids;
    list(ids);
    REQUIRE(ids.size() == 2);
    REQUIRE(ids[0] == object0.id());
    REQUIRE(ids[1] == object1.id());

    remove(object0);

    REQUIRE_FALSE(exists(object0));
    std::vector<std::string> updated_ids;
    list(updated_ids);
    REQUIRE(updated_ids.size() == 1);
}
