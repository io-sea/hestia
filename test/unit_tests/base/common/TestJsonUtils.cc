#include <catch2/catch_all.hpp>

#include "JsonUtils.h"

#include <iostream>

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

TEST_CASE("Test JsonUtils - dict to and from json", "[common]")
{
    hestia::Dictionary dict;
    dict.set_map({{"key0", "val0"}, {"key1", "val1"}});

    auto seq_dict = std::make_unique<hestia::Dictionary>(
        hestia::Dictionary::Type::SEQUENCE);

    auto nest0 = std::make_unique<hestia::Dictionary>();
    nest0->set_map({{"key2", "val2"}, {"key3", "val4"}});

    seq_dict->add_sequence_item(std::move(nest0));
    dict.set_map_item("nested", std::move(seq_dict));

    auto json = hestia::JsonUtils::to_json(dict);

    auto ret_dict = hestia::JsonUtils::from_json(json);
    REQUIRE(ret_dict->has_map_item("nested"));

    auto nested_seq = ret_dict->get_map_item("nested");
    REQUIRE(nested_seq->get_type() == hestia::Dictionary::Type::SEQUENCE);
    REQUIRE(nested_seq->get_sequence().size() == 1);

    auto json_rebuilt = hestia::JsonUtils::to_json(*ret_dict);
    REQUIRE(json_rebuilt == json);
}
