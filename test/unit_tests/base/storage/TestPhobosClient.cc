#include <catch2/catch_all.hpp>

#include "FifoStreamSink.h"
#include "FifoStreamSource.h"

#include "Logger.h"
#include "ObjectStoreTestWrapper.h"

#include <unordered_map>

class PhobosStoreTestFixture : public ObjectStoreTestWrapper {
  public:
    PhobosStoreTestFixture() : ObjectStoreTestWrapper("mock_phobos_plugin") {}
};

TEST_CASE_METHOD(PhobosStoreTestFixture, "Test phobos object store", "[phobos]")
{
    hestia::StorageObject obj("0000");
    obj.set_metadata("mykey", "myval");

    exists(obj, false);

    put(obj);

    exists(obj, true);

    hestia::StorageObject fetched_obj("0000");
    get(fetched_obj);

    REQUIRE(fetched_obj.get_metadata_item("mykey") == "myval");

    remove(obj);
    exists(fetched_obj, false);

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.size());

    put(obj, content);

    std::string returned_content;
    get(obj, returned_content, content.size());
    REQUIRE(returned_content == content);

    std::vector<hestia::StorageObject> fetched_objects;
    list({"mykey", "myval"}, fetched_objects);
    REQUIRE(fetched_objects.size() == 1);
    REQUIRE(fetched_objects[0].id() == obj.id());
}