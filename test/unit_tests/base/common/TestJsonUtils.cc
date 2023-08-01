#include <catch2/catch_all.hpp>

#include "JsonUtils.h"
#include "TestUtils.h"

#include <fstream>
#include <iostream>

TEST_CASE("Test JsonUtils - read/write", "[common]")
{
    auto test_output_dir = TestUtils::get_test_output_dir();
    auto json_file_path   = test_output_dir / "json_file.txt";

    hestia::Map metadata;
    metadata.set_item("my_key0", "my_val0");
    metadata.set_item("my_key1", "my_val1");
    hestia::JsonUtils::write(json_file_path, metadata, 0);
    metadata.set_item("my_key2", "my_val2");
    hestia::JsonUtils::write(json_file_path, metadata, 1);

    hestia::Map expected_metadata;
    hestia::JsonUtils::read(json_file_path, expected_metadata, {"my_key0", "my_key1", "my_key2"});
    REQUIRE(expected_metadata.get_item("my_key0") == "my_val0");
    REQUIRE(expected_metadata.get_item("my_key1") == "my_val1");
    REQUIRE(expected_metadata.get_item("my_key2") == "my_val2");
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

    auto test_output_dir = TestUtils::get_test_output_dir();
    auto json_file_path   = test_output_dir / "json_file.txt";
    //std::ofstream json_file;
    //hestia::Map metadata;
    //metadata.set_item("my_key0", "my_val0");
    //hestia::JsonUtils::write(json_file_path, metadata, 0);
    
    //std::vector<hestia::KeyValuePair> kv_pairs;
    //kv_pairs={{"my_key1", "my_val1"}};//, {"my_key2", "my_val2"}};
    hestia::JsonUtils::set_values(json_file_path, {{"my_key1", "my_val1"}});

    /*std::vector<std::string> given_keys = {"my_key0"};
    std::vector<std::string> expected_values;
    hestia::JsonUtils::get_values(json_file_path, given_keys, expected_values);
    REQUIRE(expected_values[0] == "my_val0");
    */
    //hestia::JsonUtils::set_value(json_file_path, "my_key0", "my_new_val0");
    
    /*std::string given_key = {"my_key0"};
    std::string expected_value;
    bool value=hestia::JsonUtils::get_value(json_file_path, given_key, expected_value);
    REQUIRE(value == 1);
    REQUIRE(expected_value == "my_new_val0");*/
}

TEST_CASE("Test JsonUtils - has/remove keys", "[common]")
{
    auto test_output_dir = TestUtils::get_test_output_dir();
    auto json_file_path   = test_output_dir / "json_file.txt";
    std::ofstream json_file;
    hestia::Map metadata;
    metadata.set_item("my_key0", "my_val0");
    metadata.set_item("my_key1", "my_val1");
    metadata.set_item("my_key2", "my_val2");
    hestia::JsonUtils::write(json_file_path, metadata, 0);

    bool value_found=hestia::JsonUtils::has_key(json_file_path, {"my_key0"});
    REQUIRE(value_found == 1);

    std::vector<bool> expected_found;
    hestia::JsonUtils::has_keys(json_file_path, {"my_key0", "my_key1"}, expected_found);
    REQUIRE(expected_found[0] == 1);
    REQUIRE(expected_found[1] == 1);

    hestia::JsonUtils::remove_key(json_file_path, {"my_key0"});
    value_found=hestia::JsonUtils::has_key(json_file_path, {"my_key0"});
    REQUIRE(value_found == 0);

    std::vector<bool> expected_founds;
    hestia::JsonUtils::remove_keys(json_file_path, {"my_key1", "my_key2"});
    hestia::JsonUtils::has_keys(json_file_path, {"my_key1", "my_key2"}, expected_founds);
    REQUIRE(expected_founds[0] == 0);
    REQUIRE(expected_founds[1] == 0);
}

TEST_CASE("Test JsonUtils - metadata to and from json", "[common]")
{
    hestia::Map metadata;
    metadata.set_item("my_key0", "my_val0");
    metadata.set_item("my_key1", "my_val1");
    const auto as_json = hestia::JsonUtils::to_json(metadata);
    std::cout<<"metadata: "<<metadata<<std::endl;
    std::cout<<"as_json: "<<as_json<<std::endl;

    hestia::Map expected_metadata;
    hestia::JsonUtils::from_json(as_json, expected_metadata);
    REQUIRE(expected_metadata.get_item("my_key0") == "my_val0");
    REQUIRE(expected_metadata.get_item("my_key1") == "my_val1");
    std::cout<<"expected_metadata: "<<expected_metadata<<std::endl;

    std::cout<<"----------------------"<<std::endl;
    std::vector<hestia::Map> metadata_vector;
    metadata_vector.push_back(expected_metadata);
    //hestia::Map metadata2;
    //metadata2.set_item("my_key2", "my_val2");
    //metadata_vector.push_back(metadata2);
 
    std::cout<<"[0]: "<<metadata_vector[0]<<std::endl; 
    for (const auto& md : metadata_vector) {
        std::cout<<md<<std::endl;
    }
    const auto as_json_vector = hestia::JsonUtils::to_json(metadata_vector);//, "KEY0");
    std::cout<<"as_json_vector: "<<as_json_vector<<std::endl; 

    std::cout<<"----------------------"<<std::endl;
    std::vector<hestia::Map> expected_metadata_vector;
    hestia::JsonUtils::from_json(as_json_vector, expected_metadata_vector);
    
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

    std::string json;
    hestia::JsonUtils::to_json(dict, json);

    hestia::Dictionary ret_dict;
    hestia::JsonUtils::from_json(json, ret_dict);
    REQUIRE(ret_dict.has_map_item("nested"));

    auto nested_seq = ret_dict.get_map_item("nested");
    REQUIRE(nested_seq->get_type() == hestia::Dictionary::Type::SEQUENCE);
    REQUIRE(nested_seq->get_sequence().size() == 1);

    std::string json_rebuilt;
    hestia::JsonUtils::to_json(ret_dict, json_rebuilt);
    REQUIRE(json_rebuilt == json);
}
