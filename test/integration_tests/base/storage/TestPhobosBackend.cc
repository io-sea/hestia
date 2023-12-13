#ifdef HAS_PHOBOS

#include <catch2/catch_all.hpp>

#include "IdGenerator.h"
#include "ObjectStoreTestWrapper.h"

class PhobosStoreTestFixture : public ObjectStoreTestWrapper {
  public:
    PhobosStoreTestFixture() : ObjectStoreTestWrapper("phobos_plugin") {}
};


TEST_CASE_METHOD(
    PhobosStoreTestFixture, "Test phobos backend integration", "[phobos]")
{
    hestia::DefaultIdGenerator uuid_generator;
    std::string obj_id = uuid_generator.get_id("phobos_test");

    hestia::StorageObject obj(obj_id);
    obj.get_metadata_as_writeable().set_item("mykey", "myval");

    exists(obj, false);

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.size());

    put(obj, content);

    std::string returned_content;
    get(obj, returned_content, content.length());
    REQUIRE(returned_content == content);

    std::vector<hestia::StorageObject> fetched_objects;
    list({"mykey", "myval"}, fetched_objects);
    REQUIRE(fetched_objects.size() == 1);
    REQUIRE(fetched_objects[0].id() == obj.id());

    remove(obj);
    exists(obj, false);
}

#endif