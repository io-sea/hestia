#include <catch2/catch_all.hpp>

#include "ScalarField.h"
#include "Uuid.h"
#include "UuidUtils.h"
#include <stdint.h>

TEST_CASE("Test Uuid - to string", "[uuid]")
{
    REQUIRE(
        hestia::UuidUtils::to_string(hestia::Uuid(0))
        == "00000000-0000-0000-0000-000000000000");

    REQUIRE(
        hestia::UuidUtils::to_string(hestia::Uuid(5))
        == "00000000-0000-0000-0000-000000000005");

    REQUIRE(
        hestia::UuidUtils::to_string(hestia::Uuid(500))
        == "00000000-0000-0000-0000-0000000001f4");

    REQUIRE(
        hestia::UuidUtils::to_string(hestia::Uuid(UINT64_MAX))
        == "00000000-0000-0000-ffff-ffffffffffff");

    REQUIRE(
        hestia::UuidUtils::to_string(hestia::Uuid(UINT64_MAX, 1))
        == "00000000-0000-0001-ffff-ffffffffffff");

    REQUIRE(
        hestia::UuidUtils::to_string(hestia::Uuid(UINT64_MAX, UINT64_MAX - 1))
        == "ffffffff-ffff-fffe-ffff-ffffffffffff");

    REQUIRE(
        hestia::UuidUtils::to_string(hestia::Uuid(UINT64_MAX, UINT64_MAX))
        == "ffffffff-ffff-ffff-ffff-ffffffffffff");
}

TEST_CASE("Test Uuid - from string", "[uuid]")
{
    REQUIRE(
        hestia::UuidUtils::from_string("00000000-0000-0000-0000-000000000000")
        == hestia::Uuid(0));

    REQUIRE(
        hestia::UuidUtils::from_string("00000000-0000-0000-0000-000000000005")
        == hestia::Uuid(5));

    REQUIRE(
        hestia::UuidUtils::from_string("00000000-0000-0000-0000-0000000001f4")
        == hestia::Uuid(500));

    REQUIRE(
        hestia::UuidUtils::from_string("00000000-0000-0000-ffff-ffffffffffff")
        == hestia::Uuid(UINT64_MAX));

    REQUIRE(
        hestia::UuidUtils::from_string("00000000-0000-0001-ffff-ffffffffffff")
        == hestia::Uuid(UINT64_MAX, 1));

    REQUIRE(
        hestia::UuidUtils::from_string("ffffffff-ffff-fffe-ffff-ffffffffffff")
        == hestia::Uuid(UINT64_MAX, UINT64_MAX - 1));

    REQUIRE(
        hestia::UuidUtils::from_string("ffffffff-ffff-ffff-ffff-ffffffffffff")
        == hestia::Uuid(UINT64_MAX, UINT64_MAX));
}

TEST_CASE("Test Uuid - set or unset", "[uuid]")
{
    REQUIRE(hestia::Uuid().is_unset());
    REQUIRE(!hestia::Uuid(0).is_unset());
}

TEST_CASE("Test UuidField", "[uuid]")
{
    hestia::UuidField testfield{"UuidTestfield"};
    testfield.value_from_string("00000000-0000-0000-0000-0000000001f4");
    REQUIRE(
        testfield.value_as_string() == "00000000-0000-0000-0000-0000000001f4");
}