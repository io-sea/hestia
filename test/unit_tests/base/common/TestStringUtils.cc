#include <catch2/catch_all.hpp>

#include "StringUtils.h"

TEST_CASE("Test StringUtils - int conversion", "[common]")
{
    REQUIRE(hestia::StringUtils::to_int("12") == 12);

    REQUIRE(hestia::StringUtils::to_int("ABC123", true) == 11256099);

    REQUIRE(hestia::StringUtils::id_to_string(123, 456, '_') == "7b_1c8");

    REQUIRE(
        hestia::StringUtils::id_to_string(123, 456, '_', false) == "123_456");

    const auto& [first, second] =
        hestia::StringUtils::split_on_first("quick_brown_fox", '_');
    REQUIRE(first == "quick");
    REQUIRE(second == "brown_fox");

    REQUIRE(hestia::StringUtils::to_lower("QuickBrownFox") == "quickbrownfox");
}

TEST_CASE("Test StringUtils - Remove prefix", "[common]")
{
    std::string test_string = "hestia::tiers";
    std::string prefix      = "hestia::";
    auto without_prefix =
        hestia::StringUtils::remove_prefix(test_string, prefix);
    REQUIRE(without_prefix == "tiers");
}