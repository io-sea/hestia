#include <catch2/catch_all.hpp>

#include "TestUtils.h"
#include "YamlUtils.h"

TEST_CASE("Test YamlUtils - get values", "[common]")
{
    auto test_input_dir = TestUtils::get_test_data_dir();
    auto yaml_path      = test_input_dir / "test.yaml";

    hestia::NestedMetadata data;

    hestia::YamlUtils::load(yaml_path.string(), data);

    REQUIRE(data.get_item("key1") == "value1");
}