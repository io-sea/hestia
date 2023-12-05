#include "TimeUtils.h"

#include "Logger.h"

#include <cassert>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string.h>

namespace hestia {
std::time_t TimeUtils::get_current_time()
{
    const auto epoch_count =
        std::chrono::system_clock::now().time_since_epoch().count();
    return epoch_count;
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
    const auto now      = std::chrono::system_clock::now();
    const auto time_now = std::chrono::system_clock::to_time_t(now);
    return to_iso8601_basic(time_now);
}

std::string TimeUtils::to_iso8601_basic(std::time_t time)
{
    std::stringstream ss;
    errno           = 0;
    auto local_time = std::localtime(&time);
    if (local_time == nullptr) {
        std::string err_str;
        if (errno > 0) {
            err_str = strerror(errno);
        }
        LOG_ERROR(
            "Failed to convert time to localtime: " << time << " with error "
                                                    << err_str);
        return {};
    }
    ss << std::put_time(local_time, "%Y%m%dT%H%M%SZ");
    return ss.str();
}

}  // namespace hestia
