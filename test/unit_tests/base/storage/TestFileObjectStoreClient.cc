#include <catch2/catch_all.hpp>

#include "FileObjectStoreClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "ObjectStoreTestWrapper.h"
#include "TestUtils.h"

class FileObjectStoreTestFixture {
  public:
    ~FileObjectStoreTestFixture()
    {
        std::filesystem::remove_all(get_store_path());
    }

    void init(const std::string& test_name)
    {
        m_test_name           = test_name;
        const auto store_path = get_store_path();
        std::filesystem::remove_all(store_path);

        m_client = ObjectStoreTestWrapper::create(
            hestia::FileObjectStoreClient::create());

        hestia::FileObjectStoreClientConfig config;
        config.m_root = store_path;
        config.set_mode(
            hestia::FileObjectStoreClientConfig::Mode::DATA_AND_METADATA);

        hestia::Dictionary dict;
        config.serialize(dict);
        m_client->m_client->initialize("0", {}, dict);
    }

    std::string get_store_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    std::string m_test_name;
    std::unique_ptr<ObjectStoreTestWrapper> m_client;
};


TEST_CASE_METHOD(
    FileObjectStoreTestFixture, "Test local file object store", "[storage]")
{
    init("LocalFileObjectStore");

    hestia::StorageObject obj("0000");
    obj.set_metadata("mykey", "myval");

    m_client->put(obj);

    hestia::StorageObject new_obj("0000");
    m_client->get(new_obj);

    std::string objdata = "The quick brown fox jumps over the lazy dog.";
    m_client->put(obj, objdata);

    std::string fetched_data;
    m_client->get(obj, fetched_data, objdata.size());

    REQUIRE(fetched_data == objdata);

    m_client->exists(obj, true);

    m_client->remove(obj);

    m_client->exists(obj, false);
}