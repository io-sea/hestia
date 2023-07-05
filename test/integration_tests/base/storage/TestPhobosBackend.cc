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
    hestia::Uuid obj_uuid = uuid_generator.get_uuid("phobos_test");

    hestia::StorageObject obj(obj_uuid);
    obj.m_metadata.set_item("mykey", "myval");

    exists(obj, false);

    // No way of putting just metadata

    // put(obj);
    // exists(obj, true);

    // hestia::StorageObject fetched_obj(obj_uuid);
    // get(fetched_obj);

    // REQUIRE(fetched_obj.m_metadata.get_item("mykey") == "myval");

    // remove(obj);
    // exists(fetched_obj, false);

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.m_size          = content.size();

    hestia::Stream stream;
    put(obj, &stream);

    REQUIRE(stream.write(content).ok());
    REQUIRE(stream.reset().ok());

    get(obj, &stream);

    std::vector<char> returned_buffer(content.length());
    hestia::WriteableBufferView write_buffer(returned_buffer);
    REQUIRE(stream.read(write_buffer).ok());
    REQUIRE(stream.reset().ok());

    std::string returned_content =
        std::string(returned_buffer.begin(), returned_buffer.end());
    REQUIRE(returned_content == content);

    std::vector<hestia::StorageObject> fetched_objects;
    list({"mykey", "myval"}, fetched_objects);
    REQUIRE(fetched_objects.size() == 1);
    REQUIRE(fetched_objects[0].id() == obj.id());

    remove(obj);
    exists(obj, false);
}

#endif