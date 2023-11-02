#include <catch2/catch_all.hpp>

#include "hestia_iosea.h"
#include "hestia_private.h"

#include "JsonUtils.h"
#include "MockHestiaClient.h"
#include "TestClientConfigs.h"

#include <iostream>
#include <numeric>

class HestiaHLCApiTestFixture {
  public:
    HestiaHLCApiTestFixture()
    {
        hestia::Dictionary extra_config;
        hestia::TestClientConfigs::get_hsm_memory_client_config(extra_config);

        auto rc = hestia_initialize(
            nullptr, nullptr,
            hestia::JsonDocument(extra_config).to_string().c_str());
        REQUIRE(rc == 0);
    }

    ~HestiaHLCApiTestFixture()
    {
        LOG_INFO("Shutting down hestia client");
        hestia_finish();
    }

    void do_put(
        unsigned id,
        uint8_t tier,
        bool overwrite             = false,
        const std::string& content = {})
    {
        HestiaId hestia_id;
        hestia_id.m_lo = id;

        const auto create_mode = overwrite ?
                                     hestia_create_mode_t::HESTIA_UPDATE :
                                     hestia_create_mode_t::HESTIA_CREATE;
        HestiaIoContext io_context;
        int rc = 0;
        if (content.empty()) {
            io_context.m_type = hestia_io_type_t::HESTIA_IO_EMPTY;
            rc = hestia_object_put(&hestia_id, create_mode, &io_context, tier);
        }
        else {
            io_context.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
            io_context.m_length = content.size();
            std::vector<char> data(content.begin(), content.end());
            io_context.m_buffer = data.data();
            rc = hestia_object_put(&hestia_id, create_mode, &io_context, tier);
        }
        REQUIRE(rc == 0);
    }

    void do_get(unsigned id, uint8_t tier, std::string& content)
    {
        HestiaId hestia_id;
        hestia_id.m_lo = id;

        std::vector<char> buffer(content.length());
        HestiaIoContext io_context;
        io_context.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
        io_context.m_length = content.length();
        io_context.m_buffer = buffer.data();

        const auto rc = hestia_object_get(&hestia_id, &io_context, tier);
        content       = std::string(buffer.begin(), buffer.end());
        REQUIRE(rc == 0);
    }

    void get_attrs(unsigned id, HestiaObject& object)
    {
        HestiaId hestia_id;
        hestia_id.m_lo = id;
        auto rc        = hestia_object_get_attrs(&hestia_id, &object);
        REQUIRE(rc == 0);
    }

    void set_attrs(
        unsigned id,
        const std::vector<std::pair<std::string, std::string>>& attrs)
    {
        HestiaId hestia_id;
        hestia_id.m_lo = id;

        HestiaKeyValuePair* kv_pairs = new HestiaKeyValuePair[attrs.size()];
        for (std::size_t idx = 0; idx < attrs.size(); idx++) {
            hestia::StringUtils::to_char(
                attrs[idx].first, &kv_pairs[idx].m_key);
            hestia::StringUtils::to_char(
                attrs[idx].second, &kv_pairs[idx].m_value);
        }
        auto rc = hestia_object_set_attrs(&hestia_id, kv_pairs, attrs.size());
        REQUIRE(rc == 0);

        for (std::size_t idx = 0; idx < attrs.size(); idx++) {
            delete[] kv_pairs[idx].m_key;
            delete[] kv_pairs[idx].m_value;
        }
        delete[] kv_pairs;
        REQUIRE(rc == 0);
    }

    void list_tiers(std::vector<uint8_t>& tiers)
    {
        uint8_t* tier_ids{nullptr};
        std::size_t num_tier_ids{0};

        const auto rc = hestia_list_tiers(&tier_ids, &num_tier_ids);
        REQUIRE(rc == 0);

        for (std::size_t idx = 0; idx < num_tier_ids; idx++) {
            tiers.push_back(tier_ids[idx]);
        }
        delete[] tier_ids;
    }

    hestia::mock::MockHestiaClient* m_client{nullptr};
};

TEST_CASE_METHOD(HestiaHLCApiTestFixture, "Test Hestia HL C API", "[hestia]")
{
    unsigned id0{1234};
    do_put(id0, 0);

    std::vector<uint8_t> tiers;
    list_tiers(tiers);
    REQUIRE(tiers.size() == 5);

    // Check we have one of each of 0, 1, 2, 3, 4 by summing them
    REQUIRE(std::accumulate(tiers.begin(), tiers.end(), 0) == 10);

    std::vector<std::pair<std::string, std::string>> kv_pairs = {
        {"my_key0", "my_val0"}, {"my_key1", "my_val1"}, {"my_key2", "my_val2"}};
    set_attrs(id0, kv_pairs);

    std::string content("The quick brown fox jumps over the lazy dog");
    do_put(id0, 0, true, content);

    HestiaObject object;
    get_attrs(id0, object);

    REQUIRE(object.m_num_attrs == kv_pairs.size());

    REQUIRE(
        hestia::StringUtils::starts_with(object.m_attrs[0].m_key, "my_key"));
    REQUIRE(
        hestia::StringUtils::starts_with(object.m_attrs[0].m_value, "my_val"));

    REQUIRE(object.m_num_tier_extents == 1);
    REQUIRE(object.m_tier_extents[0].m_size == content.size());
    REQUIRE(object.m_size == content.size());
    hestia_init_object(&object);

    std::string returned_content(content.length(), '0');
    do_get(id0, 0, returned_content);
    REQUIRE(content == returned_content);

    HestiaId hestia_id;
    hestia_id.m_lo = id0;
    hestia_object_copy(&hestia_id, 0, 1);

    get_attrs(id0, object);
    REQUIRE(object.m_num_tier_extents == 2);
    hestia_init_object(&object);

    hestia_object_copy(&hestia_id, 1, 2);
    get_attrs(id0, object);
    REQUIRE(object.m_num_tier_extents == 3);
    hestia_init_object(&object);

    hestia_object_move(&hestia_id, 2, 3);
    get_attrs(id0, object);
    REQUIRE(object.m_num_tier_extents == 3);
    hestia_init_object(&object);

    hestia_object_release(&hestia_id, 3, 0);
    get_attrs(id0, object);
    REQUIRE(object.m_num_tier_extents == 2);
    hestia_init_object(&object);

    uint8_t* object_tiers{nullptr};
    std::size_t num_tiers{0};
    auto rc = hestia_object_locate(&hestia_id, &object_tiers, &num_tiers);
    REQUIRE(rc == 0);

    REQUIRE(num_tiers == 2);
    REQUIRE((object_tiers[0] == 0 || object_tiers[0] == 1));
    REQUIRE((object_tiers[1] == 0 || object_tiers[1] == 1));
    hestia_free_tier_ids(&object_tiers);

    return;

    HestiaId* tier_object_ids{nullptr};
    std::size_t num_tier_object_ids{0};
    rc = hestia_object_list(0, &tier_object_ids, &num_tier_object_ids);

    REQUIRE(rc == 0);

    REQUIRE(num_tier_object_ids == 1);

    hestia_free_ids(&tier_object_ids);
}