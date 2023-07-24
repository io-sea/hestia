#include <catch2/catch_all.hpp>

#include "hestia.h"

#include "JsonUtils.h"
#include "TestClientConfigs.h"

#include <iostream>

class HestiaCApiTestFixture {
  public:
    HestiaCApiTestFixture()
    {
        hestia::Dictionary extra_config;
        hestia::TestClientConfigs::get_hsm_memory_client_config(extra_config);

        std::string config_str;
        hestia::JsonUtils::to_json(extra_config, config_str);

        auto rc = hestia_initialize(nullptr, nullptr, config_str.c_str());
        REQUIRE(rc == 0);
    }

    ~HestiaCApiTestFixture() { hestia_finish(); }
};

TEST_CASE_METHOD(HestiaCApiTestFixture, "Test Hestia C API", "[hestia]")
{
    char* output{nullptr};
    int len_output{0};

    auto rc = hestia_create(
        HESTIA_OBJECT, HESTIA_IO_NONE, HESTIA_ID_NONE, nullptr, 0,
        HESTIA_IO_IDS, &output, &len_output);
    REQUIRE(rc == 0);
    REQUIRE(len_output > 0);
    REQUIRE(output != nullptr);

    std::string id(output, len_output);
    delete[] output;

    std::string content("The quick brown fox jumps over the lazy dog");
    // rc = hestia_data_put(id.c_str(), content.data(), content.length(), 0, 0);
    // REQUIRE(rc == 0);
}