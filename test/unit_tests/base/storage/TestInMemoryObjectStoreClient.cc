#include <catch2/catch_all.hpp>

#include "InMemoryObjectStoreClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "ObjectStoreTestWrapper.h"
#include "TestUtils.h"

class InMemoryStoreTestFixture : public ObjectStoreTestWrapper {
  public:
    InMemoryStoreTestFixture() :
        ObjectStoreTestWrapper(hestia::InMemoryObjectStoreClient::create())
    {
    }
};

TEST_CASE_METHOD(
    InMemoryStoreTestFixture, "In Memory object store - Metadata", "[storage]")
{
    hestia::StorageObject obj("0000");
    obj.m_metadata.set_item("mykey", "myval");

    WHEN("There is no existing object")
    {
        THEN("The exist check is false")
        {
            exists(obj, false);
        }
    }

    WHEN("We PUT an object")
    {
        put(obj);

        THEN("The exist check is true")
        {
            exists(obj, true);
        }

        WHEN("We GET the same object")
        {
            hestia::StorageObject fetched_obj("0000");
            get(fetched_obj);

            THEN("It retains the metadata")
            {
                REQUIRE(fetched_obj.m_metadata.get_item("mykey") == "myval");
            }
        }

        WHEN("We LIST with a query for the object metadata")
        {
            std::vector<hestia::StorageObject> fetched_objects;
            list({"mykey", "myval"}, fetched_objects);

            THEN("We find the object")
            {
                REQUIRE(fetched_objects.size() == 1);
                REQUIRE(fetched_objects[0].m_id == obj.m_id);
            }
        }

        WHEN("We REMOVE the object")
        {
            remove(obj);

            THEN("It no longer exists")
            {
                exists(obj, false);
            }
        }
    }
}

TEST_CASE_METHOD(
    InMemoryStoreTestFixture, "In Memory object store - Data", "[storage]")
{
    hestia::StorageObject obj("0000");
    obj.m_metadata.set_item("mykey", "myval");

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.m_size          = content.size();

    hestia::Stream stream;
    put(obj, &stream);

    WHEN("Content is passed in one chunk")
    {
        REQUIRE(stream.write(content).ok());
        REQUIRE(stream.reset().ok());

        WHEN("Content is read in one chunk")
        {
            get(obj, &stream);

            std::vector<char> returned_buffer(content.length());
            hestia::WriteableBufferView write_buffer(returned_buffer);
            REQUIRE(stream.read(write_buffer).ok());
            REQUIRE(stream.reset().ok());

            THEN("It is the same as the original content")
            {
                std::string returned_content =
                    std::string(returned_buffer.begin(), returned_buffer.end());
                REQUIRE(returned_content == content);
            }
        }
    }

    WHEN("Content is passed in multiple chunks")
    {
        std::size_t chunk_size = 10;
        std::vector<char> data_chars(content.begin(), content.end());
        std::size_t cursor = 0;
        while (cursor < content.size()) {
            auto chunk_end = cursor + chunk_size;
            if (chunk_end >= content.size()) {
                chunk_end = content.size();
            }
            if (chunk_end == cursor) {
                break;
            }
            hestia::ReadableBufferView read_buffer(
                &data_chars[0] + cursor, chunk_end - cursor);
            REQUIRE(stream.write(read_buffer).ok());
            cursor = chunk_end;
        }

        WHEN("Content is read in one chunk")
        {
            get(obj, &stream);

            std::vector<char> returned_buffer(content.length());
            hestia::WriteableBufferView write_buffer(returned_buffer);
            REQUIRE(stream.read(write_buffer).ok());
            REQUIRE(stream.reset().ok());

            THEN("It is the same as the original content")
            {
                std::string returned_content =
                    std::string(returned_buffer.begin(), returned_buffer.end());
                REQUIRE(returned_content == content);
            }
        }
    }
}