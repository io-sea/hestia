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
            hestia::FileObjectStoreClient::create(store_path));
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
    obj.m_metadata.set_item("mykey", "myval");

    m_client->put(obj);

    hestia::StorageObject new_obj("0000");
    m_client->get(new_obj);

    REQUIRE(new_obj == obj);

    std::string objdata = "The quick brown fox jumps over the lazy dog.";
    std::vector<char> buffer(objdata.begin(), objdata.end());

    hestia::Stream stream;
    stream.set_source(hestia::InMemoryStreamSource::create(buffer));

    m_client->put(obj, &stream);
    REQUIRE(stream.flush().ok());

    std::vector<char> input_buffer(buffer.size());
    stream.set_sink(hestia::InMemoryStreamSink::create(input_buffer));

    m_client->get(obj, &stream);
    REQUIRE(stream.flush().ok());

    std::string obj_content(input_buffer.begin(), input_buffer.end());
    REQUIRE(obj_content == objdata);

    m_client->exists(obj, true);

    m_client->remove(obj);

    m_client->exists(obj, false);
}