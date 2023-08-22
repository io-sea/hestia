#include <catch2/catch_all.hpp>

#include "ErrorUtils.h"

TEST_CASE("Test ErrorUtils - OpStatus", "[common]")
{

    REQUIRE(
        hestia::OpStatus::get_location_prefix("main.cc", "main", 9)
        == "main::main::9");

    hestia::OpStatus status;
    status = hestia::OpStatus(hestia::OpStatus::Status::OK, 1, "Success");
    REQUIRE(status.ok());
    REQUIRE(status.code() == "1");
    REQUIRE(status.message() == "Success");
    REQUIRE(status.str() == "OK");
}
