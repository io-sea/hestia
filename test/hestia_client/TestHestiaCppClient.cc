#include <catch2/catch_all.hpp>

#include "hestia.h"

#include <string>

TEST_CASE("Test Hestia CPP client", "[.][hestia_cpp]")
{
    auto obj_id = hestia::hsm_uint{0000, 0001};

    std::string content = "The quick brown fox jumps over the lazy dog.";

    auto rc = hestia::put(obj_id, true, content.data(), 0, content.length(), 0);
    REQUIRE(rc == 0);
}