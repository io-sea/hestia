#include <catch2/catch_all.hpp>

#include "HsmObjectStoreTestWrapper.h"
#include "TestContext.h"

#include "MotrConfig.h"

#include <iostream>

class MockMotrTestFixture : public HsmObjectStoreTestWrapper {
  public:
    MockMotrTestFixture() : HsmObjectStoreTestWrapper("mock_motr_plugin")
    {
        hestia::MotrConfig config;

        hestia::MotrHsmTierInfo tier0_info;
        tier0_info.set_identifier("1");
        hestia::MotrHsmTierInfo tier1_info;
        tier1_info.set_identifier("2");
        hestia::MotrHsmTierInfo tier2_info;
        tier2_info.set_identifier("3");

        config.add_tier_info(tier0_info);
        config.add_tier_info(tier1_info);
        config.add_tier_info(tier2_info);

        hestia::Dictionary dict;
        config.serialize(dict);
        m_client->initialize("0000", {}, dict);
    }
};

TEST_CASE_METHOD(MockMotrTestFixture, "Motr client write and read", "[motr]")
{
    hestia::Uuid uuid(0);

    hestia::StorageObject obj(uuid.to_string());

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.length());

    put(obj, content, "1");

    std::string tier_1_content;
    get(obj, tier_1_content, content.length(), "1");
    REQUIRE(tier_1_content == content);

    copy(obj, "1", "2");

    std::string tier_2_content;
    get(obj, tier_2_content, content.length(), "2");
    REQUIRE(tier_2_content == content);
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