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
    obj.set_metadata("mykey", "myval");

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
                REQUIRE(fetched_obj.get_metadata_item("mykey") == "myval");
            }
        }

        WHEN("We LIST with a query for the object metadata")
        {
            std::vector<hestia::StorageObject> fetched_objects;
            list({"mykey", "myval"}, fetched_objects);

            THEN("We find the object")
            {
                REQUIRE(fetched_objects.size() == 1);
                REQUIRE(fetched_objects[0].id() == obj.id());
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
    obj.set_metadata("mykey", "myval");

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.size());

    WHEN("Content is passed in one chunk")
    {
        put(obj, content);

        WHEN("Content is read in one chunk")
        {
            std::string returned_content;
            get(obj, returned_content, content.length());

            THEN("It is the same as the original content")
            {
                REQUIRE(returned_content == content);
            }
        }
    }
}