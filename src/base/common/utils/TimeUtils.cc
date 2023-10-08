#include "TimeUtils.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace hestia {
std::time_t TimeUtils::get_current_time()
{
    return std::chrono::system_clock::now().time_since_epoch().count();
}

std::string TimeUtils::get_current_time_hr()
{
    const auto now  = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%F_%H-%M-%S");
    return ss.str();
}

std::string TimeUtils::get_current_time_iso8601_basic()
{
    const auto now = std::chrono::system_clock::now();
    return to_iso8601_basic(std::chrono::system_clock::to_time_t(now));
}

std::string TimeUtils::to_iso8601_basic(std::time_t time)
{
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%dT%H%M%SZ");
    return ss.str();
}

}  // namespace hestia
