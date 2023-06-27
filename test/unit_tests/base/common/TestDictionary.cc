#include <catch2/catch_all.hpp>

#include "Dictionary.h"

TEST_CASE("Test Dictionary - Nested queries", "[dictionary]")
{
    hestia::Dictionary dict;

    std::unordered_map<std::string, std::string> data{{"mykey", "myval"}};

    dict.set_map(data);

    hestia::Metadata::Query query{"mykey", "myval"};

    REQUIRE(dict.has_key_and_value(query));
}
