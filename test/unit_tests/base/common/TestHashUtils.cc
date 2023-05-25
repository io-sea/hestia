#include <catch2/catch_all.hpp>

#include "HashUtils.h"

TEST_CASE("Test Hashing", "[hash]")
{
    auto sha256 = hestia::HashUtils::do_sha256("Test");
}

TEST_CASE("String encoding", "[hash]")
{
    WHEN("The string contains only allowed characters")
    {
        std::string string_to_encode = "This-is_1.Normal~string";
        THEN("Nothing will be changed")
        {
            std::string encoded =
                hestia::HashUtils::uri_encode(string_to_encode, false);
            REQUIRE(encoded == string_to_encode);
        }
    }
    WHEN("The string contains a space")
    {
        std::string string_to_encode = "Before after";
        THEN("The space will be replaced by %20")
        {
            std::string encoded =
                hestia::HashUtils::uri_encode(string_to_encode, false);
            REQUIRE(encoded == "Before%20after");
        }
    }
    WHEN("The string contains special characters")
    {
        std::string string_to_encode = "text^*:text";
        THEN("The characters will be replaced by '%' and their hex values")
        {
            std::string encoded =
                hestia::HashUtils::uri_encode(string_to_encode, false);
            REQUIRE(encoded == "text%5E%2A%3Atext");
        }
    }
    WHEN("A containting slash should be encoded")
    {
        std::string string_to_encode = "Before/After";
        THEN("The '/' will be replaced by %2F")
        {
            std::string encoded =
                hestia::HashUtils::uri_encode(string_to_encode, true);
            REQUIRE(encoded == "Before%2FAfter");
        }
    }
    WHEN("A containting slash should not be encoded")
    {
        std::string string_to_encode = "Before/After";
        THEN("Nothing will be changed")
        {
            std::string encoded =
                hestia::HashUtils::uri_encode(string_to_encode, false);
            REQUIRE(encoded == string_to_encode);
        }
    }
}
