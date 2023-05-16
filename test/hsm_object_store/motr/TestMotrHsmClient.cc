#include <catch2/catch_all.hpp>

#include "MockMotrClient.h"
#include "MockMotrInterfaceImpl.h"
#include "MotrHsmClient.h"
#include "MotrInterface.h"
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

TEST_CASE("Motr client write and read", "[.][motr]")
{

    hestia::mock::MockMotrHsmClient motr_client;

    std::string id = "0";
    hestia::StorageObject obj{id};
    // hestia::Uuid objId{0};
    std::string content = "The quick brown fox jumps over the lazy dog.";


    hestia::HsmObjectStoreRequest put_op(
        obj.m_id, hestia::HsmObjectStoreRequestMethod::PUT);
    // put_op.mType = hestia::HsmObjectStoreRequestMethod::PUT;
    // put_op.mExtent = {0, content.size()};

    hestia::ReadableBufferView read_buffer(content);

    hestia::Stream stream;
    REQUIRE(stream.write(content).ok());

    /*auto rc = */ motr_client.put(put_op, &stream);
    // REQUIRE_FALSE(rc);

    REQUIRE(stream.reset().ok());

    std::vector<char> sink(content.length());
    hestia::WriteableBufferView write_buffer(sink);

    hestia::HsmObjectStoreRequest get_op(
        obj.m_id, hestia::HsmObjectStoreRequestMethod::GET);
    // get_op.mType = hestia::HsmObjectStoreRequestMethod::GET;
    // get_op.mExtent = {0, content.size()};

    /*rc = */ motr_client.get(get_op, obj, &stream);
    // REQUIRE_FALSE(rc);

    std::string recontstructed_content(sink.begin(), sink.end());
    REQUIRE(recontstructed_content == content);

    REQUIRE(false);
}
