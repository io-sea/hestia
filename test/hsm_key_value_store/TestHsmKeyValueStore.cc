#include <catch2/catch_all.hpp>

#include "FileKeyValueStoreClient.h"
#include "HsmKeyValueStore.h"
#include "TestUtils.h"

#include <filesystem>

class HsmKeyValueStoreTestFixture {
  public:
    void init(const std::string& test_name)
    {
        m_test_name           = test_name;
        const auto store_path = get_store_path();
        std::filesystem::remove_all(store_path);

        hestia::Metadata config;
        config.set_item("root", store_path);
        auto kv_store_client =
            std::make_unique<hestia::FileKeyValueStoreClient>();
        kv_store_client->initialize(config);
        m_store = std::make_unique<hestia::HsmKeyValueStore>(
            std::move(kv_store_client));
    }

    ~HsmKeyValueStoreTestFixture()
    {
        std::filesystem::remove_all(get_store_path());
    }

    void put(const hestia::StorageObject& obj)
    {
        hestia::ObjectStoreRequest request(
            obj, hestia::ObjectStoreRequestMethod::PUT);
        auto response = m_store->make_request(request);
        REQUIRE(response->ok());
    }

    void get(hestia::StorageObject& obj)
    {
        hestia::ObjectStoreRequest request(
            obj, hestia::ObjectStoreRequestMethod::GET);
        auto response = m_store->make_request(request);
        REQUIRE(response->ok());
        obj = response->object();
    }

    bool exists(const hestia::StorageObject& obj)
    {
        hestia::ObjectStoreRequest request(
            obj, hestia::ObjectStoreRequestMethod::EXISTS);
        auto response = m_store->make_request(request);
        REQUIRE(response->ok());
        return response->object_found();
    }

    void list(std::vector<hestia::StorageObject>& objects)
    {
        hestia::ObjectStoreRequest request(
            hestia::StorageObject(), hestia::ObjectStoreRequestMethod::LIST);
        auto response = m_store->make_request(request);
        REQUIRE(response->ok());
        objects = response->objects();
    }

    void remove(const hestia::StorageObject& obj)
    {
        hestia::ObjectStoreRequest request(
            obj, hestia::ObjectStoreRequestMethod::REMOVE);
        auto response = m_store->make_request(request);
        REQUIRE(response->ok());
    }

    std::string get_store_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    std::string m_test_name;
    std::unique_ptr<hestia::HsmKeyValueStore> m_store;
};

TEST_CASE_METHOD(
    HsmKeyValueStoreTestFixture,
    "Test HSM Key Value Store - Get and Set",
    "[hsm_key_value]")
{
    init("TestHsmKeyValueStore");

    hestia::StorageObject object0("01234");
    object0.m_metadata.set_item("key0", "value0");
    object0.m_metadata.set_item("key1", "value1");
    object0.m_metadata.set_item("key2", "value2");

    hestia::StorageObject object1("56789");
    hestia::StorageObject object2("abcde");

    put(object0);
    put(object1);

    REQUIRE(exists(object0));
    REQUIRE(exists(object1));
    REQUIRE_FALSE(exists(object2));

    hestia::StorageObject retrieved_object0("01234");
    get(retrieved_object0);
    REQUIRE(retrieved_object0.m_metadata.get_item("key0") == "value0");
    REQUIRE(retrieved_object0.m_metadata.get_item("key1") == "value1");
    REQUIRE(retrieved_object0.m_metadata.get_item("key2") == "value2");

    std::vector<hestia::StorageObject> objects;
    list(objects);
    REQUIRE(objects.size() == 2);
    REQUIRE(objects[0].id() == object0.id());
    REQUIRE(objects[1].id() == object1.id());

    remove(object0);

    REQUIRE_FALSE(exists(object0));
    std::vector<hestia::StorageObject> updated_objects;
    list(updated_objects);
    REQUIRE(updated_objects.size() == 1);
}
