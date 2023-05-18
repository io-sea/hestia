#include <catch2/catch_all.hpp>

//#include "MockMotrClient.h"
//#include "MockMotrInterfaceImpl.h"
//#include "MotrHsmClient.h"
//#include "MotrInterface.h"
#include "HsmObjectStoreTestWrapper.h"
#include "TestContext.h"

#include "types.h"

/*
class TestMockMotrHsmClient : public hestia::mock::MockMotrHsmClient
{
public:
    TestMockMotrHsmClient()
        : MockMotrHsmClient()
    {
    //   auto mock_impl = std::make_unique<hestia::MockMotrInterfaceImpl>();
    //    mMockMotrImpl = mock_impl.get();
    //    mMotrInterface =
std::make_unique<hestia::MotrInterface>(std::move(mock_impl));
    }

    ~TestMockMotrHsmClient

//private:
  //  hestia::MockMotrInterfaceImpl* mMockMotrImpl{nullptr};
};
*/

class MockMotrTestFixture : public HsmObjectStoreTestWrapper {
  public:
    MockMotrTestFixture() : HsmObjectStoreTestWrapper("mock_motr_plugin") {}
};

TEST_CASE_METHOD(MockMotrTestFixture, "Motr client write and read", "[.][motr]")
{

    hestia::StorageObject obj("0000");
    obj.m_metadata.set_item("mykey", "myval");

    exists(obj, false);

    put(obj);

    exists(obj, true);

    hestia::StorageObject fetched_obj("0000");
    get(fetched_obj);

    REQUIRE(fetched_obj.m_metadata.get_item("mykey") == "myval");

    remove(obj);
    exists(fetched_obj, false);

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
    REQUIRE(fetched_objects[0].m_id == obj.m_id);
}
