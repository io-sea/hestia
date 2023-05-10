#include <catch2/catch_all.hpp>

#include "Dictionary.h"
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

    auto object_store_config = root->get_map_item("object_store_clients");
    REQUIRE(object_store_config);
    REQUIRE(
        object_store_config->get_type() == hestia::Dictionary::Type::SEQUENCE);

    REQUIRE(object_store_config->get_sequence().size() == 7);
    REQUIRE(
        object_store_config->get_sequence()[0]->get_type()
        == hestia::Dictionary::Type::MAP);
    REQUIRE(
        object_store_config->get_sequence()[0]
            ->get_map_item("identifier")
            ->get_scalar()
        == "hestia::FileHsmObjectStoreClient");
}