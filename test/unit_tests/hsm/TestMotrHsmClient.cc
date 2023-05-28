#include <catch2/catch_all.hpp>

#include "HsmObjectStoreTestWrapper.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "TestContext.h"

#include "MotrConfig.h"

#include "hestia.h"

class MockMotrTestFixture : public HsmObjectStoreTestWrapper {
  public:
    MockMotrTestFixture() : HsmObjectStoreTestWrapper("mock_motr_plugin")
    {
        hestia::Metadata config;
        config.set_item("tier_info", "identifier=1;identifier=2;identifier=3");

        m_client->initialize(config);
    }

    void get_and_check(
        const std::string& obj_id, const std::string& content, uint8_t tier)
    {
        hestia::Stream stream;
        std::vector<char> returned_buffer(content.length());
        hestia::WriteableBufferView write_buffer(returned_buffer);
        auto sink = hestia::InMemoryStreamSink::create(write_buffer);
        stream.set_sink(std::move(sink));

        hestia::StorageObject obj(obj_id);
        obj.m_size = content.size();
        get(obj, &stream, tier);
        REQUIRE(stream.flush().ok());

        std::string returned_content =
            std::string(returned_buffer.begin(), returned_buffer.end());
        REQUIRE(returned_content == content);
    }
};

TEST_CASE_METHOD(MockMotrTestFixture, "Motr client write and read", "[motr]")
{
    hestia::StorageObject obj("0000");

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.m_size          = content.size();

    hestia::Stream stream;
    auto source = hestia::InMemoryStreamSource::create(
        hestia::ReadableBufferView(content));
    stream.set_source(std::move(source));

    put(obj, &stream, 1);
    REQUIRE(stream.flush().ok());

    get_and_check("0000", content, 1);

    copy(obj, 1, 2);

    get_and_check("0000", content, 2);
}

TEST_CASE("Test Motr Config", "[motr]")
{
    hestia::Metadata data;
    data.set_item(
        "tier_info",
        "name=M0_POOL_TIER1,identifier=<0x6f00000000000001:0x0>;name=M0_POOL_TIER2,identifier=<0x6f00000000000001:0x1>;name=M0_POOL_TIER3,identifier=<0x6f00000000000001:0x2>");

    hestia::MotrConfig config;
    config.from_config(data);

    REQUIRE(config.m_tier_info.size() == 3);
    REQUIRE(config.m_tier_info[0].m_identifier == "<0x6f00000000000001:0x0>");
    REQUIRE(config.m_tier_info[0].m_name == "M0_POOL_TIER1");
}