#include <catch2/catch_all.hpp>
#include <fstream>

#include "Dictionary.h"
#include "Logger.h"
#include "TestUtils.h"
#include "YamlUtils.h"

TEST_CASE("Test YamlUtils - basic", "[common]")
{
    auto test_input_dir = TestUtils::get_test_data_dir();
    auto yaml_path      = test_input_dir / "test.yaml";

    hestia::Dictionary dict;
    hestia::YamlUtils::load(yaml_path.string(), dict);

    auto root = dict.get_map_item("root");
    REQUIRE(root);
    REQUIRE(root->get_type() == hestia::Dictionary::Type::SEQUENCE);

    REQUIRE(root->get_sequence().size() == 3);
    REQUIRE(
        root->get_sequence()[0]->get_type() == hestia::Dictionary::Type::MAP);
    REQUIRE(
        root->get_sequence()[0]->get_map_item("key1")->get_scalar()
        == "value1");
}

TEST_CASE("Test YamlUtils - hestia config", "[common]")
{
    auto test_input_dir = TestUtils::get_test_data_dir();
    auto yaml_path      = test_input_dir / "configs/hestia.yaml";

    hestia::Dictionary dict;
    hestia::YamlUtils::load(yaml_path.string(), dict);

    auto root = dict.get_map_item("root");
    REQUIRE(root);
    REQUIRE(root->get_type() == hestia::Dictionary::Type::MAP);

    auto object_store_config = root->get_map_item("object_store_backends");
    REQUIRE(object_store_config);
    REQUIRE(
        object_store_config->get_type() == hestia::Dictionary::Type::SEQUENCE);

    REQUIRE(object_store_config->get_sequence().size() == 7);
    REQUIRE(
        object_store_config->get_sequence()[0]->get_type()
        == hestia::Dictionary::Type::MAP);
    REQUIRE(
        object_store_config->get_sequence()[0]
            ->get_map_item("type")
            ->get_scalar()
        == "file_hsm");
}

TEST_CASE("Yaml Utils Dict to Yaml", "[common]")
{
    auto test_input_dir  = TestUtils::get_test_data_dir();
    auto test_output_dir = TestUtils::get_test_output_dir();
    auto yaml_path       = test_input_dir / "test.yaml";
    auto out_yaml_path   = test_output_dir / "test_out.yaml";

    hestia::Dictionary dict, out_dict;
    hestia::YamlUtils::load(yaml_path.string(), dict);

    std::ofstream out_file;
    std::string out_yaml;
    hestia::YamlUtils::dict_to_yaml(dict, out_yaml);

    out_file.open(out_yaml_path.string(), std::ios::trunc);  // Clear file
    out_file << out_yaml;
    out_file.close();

    hestia::YamlUtils::load(out_yaml_path.string(), out_dict);
    LOG_DEBUG("Serialized Event : " + out_yaml);
    REQUIRE(
        dict.to_string(true) == out_dict.to_string(true));  // Deep comparison
}