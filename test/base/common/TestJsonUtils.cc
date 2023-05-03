#include <catch2/catch_all.hpp>

#include "JsonUtils.h"

TEST_CASE("Test JsonUtils - get values", "[common]")
{
    std::string input =
        "{\"my_key0\":\"my_val0\", \"my_key1\":\"my_val1\", \"my_key2\":\"my_val2\"}";

    const auto values =
        hestia::JsonUtils::get_values(input, {"my_key0", "my_key1"});
    REQUIRE(values.size() == 2);
    REQUIRE(values[0] == "my_val0");
    REQUIRE(values[1] == "my_val1");
}

TEST_CASE("Test JsonUtils - metadata to and from json", "[common]")
{
    hestia::Metadata metadata;
    metadata.set_item("my_key0", "my_val0");
    metadata.set_item("my_key1", "my_val1");
    metadata.set_item("my_key2", "my_val2");

    const auto as_json = hestia::JsonUtils::to_json(metadata);

    hestia::Metadata expected_metadata;
    hestia::JsonUtils::from_json(as_json, expected_metadata);

    REQUIRE(expected_metadata.get_item("my_key0") == "my_val0");
    REQUIRE(expected_metadata.get_item("my_key1") == "my_val1");
}