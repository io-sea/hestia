#include <catch2/catch_all.hpp>

#include "HsmObjectStoreTestWrapper.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "TestContext.h"

#include "MotrConfig.h"

#include <iostream>

class MockMotrTestFixture : public HsmObjectStoreTestWrapper {
  public:
    MockMotrTestFixture() : HsmObjectStoreTestWrapper("mock_motr_plugin")
    {
        hestia::MotrConfig config;

        hestia::MotrHsmTierInfo tier0_info;
        tier0_info.m_identifier.update_value("1");

        hestia::MotrHsmTierInfo tier1_info;
        tier1_info.m_identifier.update_value("2");

        hestia::MotrHsmTierInfo tier2_info;
        tier2_info.m_identifier.update_value("3");

        config.m_tier_info.get_container_as_writeable().push_back(tier0_info);
        config.m_tier_info.get_container_as_writeable().push_back(tier1_info);
        config.m_tier_info.get_container_as_writeable().push_back(tier2_info);

        hestia::Dictionary dict;
        config.serialize(dict);
        m_client->initialize("0000", {}, dict);
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
        obj.set_size(content.size());

        get(obj, &stream, tier);
        REQUIRE(stream.flush().ok());

        std::string returned_content =
            std::string(returned_buffer.begin(), returned_buffer.end());
        REQUIRE(returned_content == content);
    }
};

TEST_CASE_METHOD(MockMotrTestFixture, "Motr client write and read", "[motr]")
{
    hestia::Uuid uuid(0);

    hestia::StorageObject obj(uuid.to_string());

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.size());

    hestia::Stream stream;
    auto source = hestia::InMemoryStreamSource::create(
        hestia::ReadableBufferView(content));
    stream.set_source(std::move(source));

    put(obj, &stream, 1);

    REQUIRE(stream.flush().ok());

    get_and_check(uuid.to_string(), content, 1);

    copy(obj, 1, 2);

    get_and_check(uuid.to_string(), content, 2);
}

/*
TEST_CASE("Test Motr Config", "[motr]")
{
    hestia::Map data;
    data.set_item(
        "tier_info",
        "name=M0_POOL_TIER1,identifier=<0x6f00000000000001:0x0>;name=M0_POOL_TIER2,identifier=<0x6f00000000000001:0x1>;name=M0_POOL_TIER3,identifier=<0x6f00000000000001:0x2>");

    hestia::MotrConfig config;
    config.from_config(data);

    REQUIRE(config.m_tier_info.size() == 3);
    REQUIRE(config.m_tier_info[0].m_identifier == "<0x6f00000000000001:0x0>");
    REQUIRE(config.m_tier_info[0].m_name == "M0_POOL_TIER1");
}
*/