#include <catch2/catch_all.hpp>

#include "EnumUtils.h"

namespace hestia {
STRINGABLE_ENUM(Test, TYPE1, TYPE2, TYPE3, TYPE4)
}

TEST_CASE("Test EnumUtils - To and from string", "[common]")
{
    hestia::Test_enum_string_converter converter;
    converter.init();

    REQUIRE(converter.to_string(hestia::Test::TYPE1) == "type1");
    REQUIRE(converter.to_string(hestia::Test::TYPE2) == "type2");
    REQUIRE(converter.to_string(hestia::Test::TYPE3) == "type3");
    REQUIRE(converter.to_string(hestia::Test::TYPE4) == "type4");

    REQUIRE(converter.from_string("type4") == hestia::Test::TYPE4);
    REQUIRE(converter.from_string("type3") == hestia::Test::TYPE3);
    REQUIRE(converter.from_string("type2") == hestia::Test::TYPE2);
    REQUIRE(converter.from_string("type1") == hestia::Test::TYPE1);
}