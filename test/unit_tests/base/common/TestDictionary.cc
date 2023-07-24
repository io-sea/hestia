#include <catch2/catch_all.hpp>

#include "Dictionary.h"

#include <iostream>

TEST_CASE("Test Dictionary - Nested queries", "[dictionary]")
{
    hestia::Dictionary dict;

    std::unordered_map<std::string, std::string> data{{"mykey", "myval"}};

    dict.set_map(data);

    hestia::KeyValuePair query{"mykey", "myval"};

    REQUIRE(dict.has_key_and_value(query));
}

TEST_CASE("Test Dictionary - Merge", "[dictionary]")
{
    hestia::Dictionary dict0;
    std::unordered_map<std::string, std::string> data0{{"mykey0", "myval0"}};
    dict0.set_map(data0);

    hestia::Dictionary dict1;
    std::unordered_map<std::string, std::string> data1{{"mykey1", "myval1"}};
    dict1.set_map(data1);

    dict0.merge(dict1);

    REQUIRE(dict0.get_map().size() == 2);
    REQUIRE(dict0.get_map_item("mykey1")->get_scalar() == "myval1");
}

TEST_CASE("Test Dictionary - Flatten and Expand", "[dictionary]")
{
    auto dict0 = std::make_unique<hestia::Dictionary>();
    std::unordered_map<std::string, std::string> data0{
        {"mykey0", "myval0"}, {"mykey1", "myval1"}};
    dict0->set_map(data0);

    auto dict1 = std::make_unique<hestia::Dictionary>();
    std::unordered_map<std::string, std::string> data1{
        {"mykey2", "myval2"}, {"mykey3", "myval3"}};
    dict1->set_map(data1);

    auto dict2 = std::make_unique<hestia::Dictionary>();
    std::unordered_map<std::string, std::string> data2{
        {"mykey4", "myval4"}, {"mykey5", "myval5"}};
    dict2->set_map(data2);

    auto seq = std::make_unique<hestia::Dictionary>(
        hestia::Dictionary::Type::SEQUENCE);
    seq->add_sequence_item(std::move(dict0));
    seq->add_sequence_item(std::move(dict1));
    seq->add_sequence_item(std::move(dict2));

    auto outer_map = std::make_unique<hestia::Dictionary>();
    outer_map->set_map_item("mymap", std::move(seq));

    hestia::Map flattened;
    outer_map->flatten(flattened);

    hestia::Dictionary expanded;
    expanded.expand(flattened);

    REQUIRE(expanded == *outer_map);
}
