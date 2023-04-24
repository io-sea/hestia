#include <catch2/catch_all.hpp>

#include "MockMotrInterfaceImpl.h"
#include "MotrHsmClient.h"

/*
class TestMotrHsmClient : public MotrHsmClient
{
public:
    TestMotrHsmClient()
        : MotrHsmClient()
    {
        auto mock_impl = std::make_unique<MockMotrInterfaceImpl>();
        mMockMotrImpl = mock_impl.get();
        mMotrInterface = std::make_unique<MotrInterface>(std::move(mock_impl));
    }

private:
    MockMotrInterfaceImpl* mMockMotrImpl{nullptr};
};
*/

TEST_CASE("Motr client write and read", "[motr]")
{
    /*
    TestMotrHsmClient motr_client;


    hestia::hsm_uint objId{0};
    std::string content = "The quick brown fox jumps over the lazy dog.";


    ObjectOperation put_op({0, 0});
    put_op.mType = ObjectOperation::Type::PUT;
    put_op.mExtent = {0, content.size()};

    ostk::ReadableBuffer read_buffer(content);

    auto rc = motr_client.doObjectOperation(put_op, &read_buffer);
    REQUIRE_FALSE(rc);

    std::vector<char> sink(content.length());
    ostk::WriteableBuffer write_buffer(sink);

    ObjectOperation get_op({0, 0});
    get_op.mType = ObjectOperation::Type::GET;
    get_op.mExtent = {0, content.size()};

    rc = motr_client.doObjectOperation(get_op, &write_buffer);
    REQUIRE_FALSE(rc);

    std::string recontstructed_content(sink.begin(), sink.end());
    REQUIRE(recontstructed_content == content);
    */
}