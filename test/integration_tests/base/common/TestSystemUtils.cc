#include <catch2/catch_all.hpp>

#include "SystemUtils.h"

#include "Logger.h"

TEST_CASE("Test SystemUtils - Get Hostname", "[system-utils]")
{
    const auto& [status, hostname] = hestia::SystemUtils::get_hostname();
    REQUIRE(status.ok());

    LOG_INFO("Got hostname: " << hostname);
}

TEST_CASE("Test SystemUtils - Get Mac Address", "[system-utils]")
{
    const auto& [status, mac_address] = hestia::SystemUtils::get_mac_address();
    // REQUIRE(status.ok());

    LOG_INFO("Got mac_address: " << mac_address);
}