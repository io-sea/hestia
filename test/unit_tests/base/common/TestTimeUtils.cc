#include <catch2/catch_all.hpp>

#include "TimeUtils.h"

#include <ctime>
#include <iomanip>

TEST_CASE("Test TimeUtils - get current time", "[common]")
{
    std::string hestialocaltime_str = hestia::TimeUtils::get_current_time_hr();

    std::time_t t = time(0);
    std::tm local = *std::localtime(&t);
    std::stringstream localtime;
    localtime << std::put_time(&local, "%F_%H-%M-%S");
    std::string localtime_str = localtime.str();

    REQUIRE(hestialocaltime_str == localtime_str);
}
