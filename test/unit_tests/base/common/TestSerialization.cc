#include <catch2/catch_all.hpp>

#include "Dictionary.h"
#include <iostream>

#include "TestUtils.h"

TEST_CASE("Test Key Value Adapter", "[models]")
{
    hestia::Dictionary ref_dict;
    ref_dict.set_map(
        {{"my_key0", "my_val0"},
         {"my_key1", "my_val1"},
         {"my_key2", "my_val2"}});

    hestia::Dictionary input_dict(hestia::Dictionary::Type::SEQUENCE);
    input_dict.add_sequence_item(
        std::make_unique<hestia::Dictionary>(ref_dict));
    input_dict.add_sequence_item(
        std::make_unique<hestia::Dictionary>(ref_dict));
    input_dict.add_sequence_item(
        std::make_unique<hestia::Dictionary>(ref_dict));

    std::string serialized_dict = input_dict.to_string();

    hestia::Dictionary output_dict;
    output_dict.from_string(serialized_dict);

    REQUIRE(output_dict.is_sequence());
    REQUIRE(output_dict.get_sequence().size() == 3);
}