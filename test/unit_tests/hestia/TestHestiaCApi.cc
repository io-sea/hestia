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
    char* response{nullptr};
    int len_response{0};

    auto rc = hestia_create(
        HESTIA_OBJECT, HESTIA_ID_NONE, HESTIA_ATTRS_NONE, HESTIA_OUTPUT_IDS,
        nullptr, 0, &response, &len_response);
    REQUIRE(rc == 0);
    REQUIRE(len_response > 0);
    REQUIRE(response != nullptr);

    std::string id(response, len_response);
    delete[] response;

    std::string content("The quick brown fox jumps over the lazy dog");
    // rc = hestia_data_put(id.c_str(), content.data(), content.length(), 0, 0);
    // REQUIRE(rc == 0);
}