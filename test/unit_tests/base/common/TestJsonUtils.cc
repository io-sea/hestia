#include <catch2/catch_all.hpp>

#include "FileUtils.h"
#include "JsonDocument.h"
#include "JsonUtils.h"
#include "TestUtils.h"

#include <fstream>
#include <iostream>

TEST_CASE("Test JsonUtils - read/write", "[common]")
{
    auto test_output_dir = TestUtils::get_test_output_dir() / "TestJsonUtils";
    std::filesystem::create_directory(test_output_dir);

    auto json_file_path = test_output_dir / "json_file.txt";

    hestia::Map metadata;
    metadata.set_item("my_key0", "my_val0");
    metadata.set_item("my_key1", "my_val1");
    hestia::JsonUtils::write(json_file_path, metadata, false);
    metadata.set_item("my_key2", "my_val2");
    hestia::JsonUtils::write(json_file_path, metadata, true);

    hestia::Map expected_metadata;
    hestia::JsonUtils::read(
        json_file_path, expected_metadata, {"my_key0", "my_key1", "my_key2"});
    REQUIRE(expected_metadata.get_item("my_key0") == "my_val0");
    REQUIRE(expected_metadata.get_item("my_key1") == "my_val1");
    REQUIRE(expected_metadata.get_item("my_key2") == "my_val2");

    std::filesystem::remove_all(test_output_dir);
}

TEST_CASE("Test JsonUtils - set/get values", "[common]")
{
    std::string input =
        "{\"my_key0\":\"my_val0\", \"my_key1\":\"my_val1\", \"my_key2\":\"my_val2\"}";

    const auto values =
        hestia::JsonUtils::get_values(input, {"my_key0", "my_key1"});
    REQUIRE(values.size() == 2);
    REQUIRE(values[0] == "my_val0");
    REQUIRE(values[1] == "my_val1");

    auto test_output_dir = TestUtils::get_test_output_dir() / "TestJsonUtils";
    std::filesystem::create_directory(test_output_dir);
    auto json_file_path = test_output_dir / "json_file.txt";

    std::vector<hestia::KeyValuePair> kv_pairs;
    kv_pairs = {
        {"my_key0", "my_val0"}, {"my_key1", "my_val1"}, {"my_key2", "my_val2"}};
    hestia::JsonUtils::set_values(json_file_path, kv_pairs);

    std::vector<std::string> given_keys = {"my_key0", "my_key1"};
    std::vector<std::string> expected_values;
    hestia::JsonUtils::get_values(json_file_path, given_keys, expected_values);
    REQUIRE(expected_values[0] == "my_val0");
    REQUIRE(expected_values[1] == "my_val1");

    hestia::JsonUtils::set_value(json_file_path, "my_key0", "my_new_val0");

    std::string given_key = {"my_key0"};
    std::string expected_value;
    bool value =
        hestia::JsonUtils::get_value(json_file_path, given_key, expected_value);
    REQUIRE(value == 1);
    REQUIRE(expected_value == "my_new_val0");

    std::filesystem::remove_all(test_output_dir);
}

TEST_CASE("Test JsonUtils - has/remove keys", "[common]")
{
    auto test_output_dir = TestUtils::get_test_output_dir() / "TestJsonUtils";
    std::filesystem::create_directory(test_output_dir);

    auto json_file_path = test_output_dir / "json_file.txt";
    hestia::Map metadata;
    metadata.set_item("my_key0", "my_val0");
    metadata.set_item("my_key1", "my_val1");
    metadata.set_item("my_key2", "my_val2");
    hestia::JsonUtils::write(json_file_path, metadata, false);

    bool value_found = hestia::JsonUtils::has_key(json_file_path, {"my_key0"});
    REQUIRE(value_found == 1);

    std::vector<bool> expected_found;
    hestia::JsonUtils::has_keys(
        json_file_path, {"my_key0", "my_key1"}, expected_found);
    REQUIRE(expected_found[0] == 1);
    REQUIRE(expected_found[1] == 1);

    hestia::JsonUtils::remove_key(json_file_path, {"my_key0"});
    value_found = hestia::JsonUtils::has_key(json_file_path, {"my_key0"});
    REQUIRE(value_found == 0);

    std::vector<bool> expected_founds;
    hestia::JsonUtils::remove_keys(json_file_path, {"my_key1", "my_key2"});
    hestia::JsonUtils::has_keys(
        json_file_path, {"my_key1", "my_key2"}, expected_founds);
    REQUIRE(expected_founds[0] == 0);
    REQUIRE(expected_founds[1] == 0);

    std::filesystem::remove_all(test_output_dir);
}

TEST_CASE("Test JsonUtils - metadata to and from json", "[common]")
{
    hestia::Map metadata;
    metadata.set_item("my_key0", "my_val0");
    metadata.set_item("my_key1", "my_val1");
    const auto as_json = hestia::JsonUtils::to_json(metadata);

    hestia::Map expected_metadata;
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
    nest0->set_map({{"key2", "val2"}, {"key3", "val3"}});

    seq_dict->add_sequence_item(std::move(nest0));
    dict.set_map_item("nested", std::move(seq_dict));

    hestia::JsonDocument json(dict);

    hestia::Dictionary ret_dict;
    json.write(ret_dict);

    REQUIRE(ret_dict.has_map_item("nested"));

    auto nested_seq = ret_dict.get_map_item("nested");
    REQUIRE(nested_seq->is_sequence());
    REQUIRE(nested_seq->get_sequence().size() == 1);
    REQUIRE(
        nested_seq->get_sequence()[0]->get_map_item("key2")->get_scalar()
        == "val2");
    REQUIRE(
        nested_seq->get_sequence()[0]->get_map_item("key3")->get_scalar()
        == "val3");

    hestia::JsonDocument json_rebuilt(ret_dict);

    REQUIRE(json_rebuilt.to_string() == json.to_string());
}

TEST_CASE("Test JsonUtils - General dict", "[common]")
{
    std::string content = "{\"backend_type\":\"memory_hsm\", \
        \"config\": \
            {\"tiers\":[\"0\",\"1\",\"2\",\"3\",\"4\"], \
            \"type\":\"in_memory_hsm_object_store_client\"}, \
        \"creation_time\":\"1691138875711091\", \
        \"id\":\"b1523f76-b5b2-f8e1-6be0-b9e20a19a070\", \
        \"last_modified_time\":\"1691138875711091\", \
        \"node\":{\"id\":\"4f3910f0-7438-57be-b6b7-bdb19f3eb703\"}, \
        \"tier_names\":[\"0\",\"1\",\"2\",\"3\",\"4\"], \
        \"tiers\":{\"ids\":[ \
            \"43b09901-49b6-df83-3e79-a9f6f116a5f4\", \
            \"95c26482-0b3d-7723-17c8-19a419d0bd1c\", \
            \"0196124d-6c0d-bfdd-1a87-9d1d3959fea0\", \
            \"421a4b4d-5a6a-e74c-b27c-18f0b23b58b5\", \
            \"3513299b-c90a-6459-02b9-c1542668109a\"]}, \
        \"type\":\"object_store_client\"}";

    hestia::Dictionary dict;
    hestia::JsonDocument json(content);
    json.write(dict);

    REQUIRE(!dict.is_empty());
    REQUIRE(dict.has_map_item("config"));
    REQUIRE(dict.get_map_item("config")->has_map_item("tiers"));
    REQUIRE(
        dict.get_map_item("config")->get_map_item("tiers")->get_type()
        == hestia::Dictionary::Type::SEQUENCE);
    REQUIRE(
        dict.get_map_item("config")
            ->get_map_item("tiers")
            ->get_sequence()
            .size()
        == 5);
}
