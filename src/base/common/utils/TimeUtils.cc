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
std::chrono::microseconds TimeUtils::get_time_since_epoch_micros()
{
    const auto epoch_duration =
        std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        epoch_duration);
}

std::string TimeUtils::get_time_since_epoch_micross_str()
{
    return std::to_string(get_time_since_epoch_micros().count());
}

std::chrono::microseconds TimeUtils::micros_from_string(const std::string& in)
{
    return std::chrono::microseconds(std::stoull(in));
}

std::string TimeUtils::micros_to_string(std::chrono::microseconds ms)
{
    return std::to_string(ms.count());
}

std::string TimeUtils::get_current_time_readable()
{
    const auto now  = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%F_%H-%M-%S");
    return ss.str();
}

std::string TimeUtils::get_current_time_iso8601_basic()
{
    return to_iso8601_basic(get_time_since_epoch_micros());
}

std::string TimeUtils::to_iso8601_basic(
    std::chrono::microseconds ms_since_epoch)
{
    std::chrono::time_point<std::chrono::system_clock> epoch;
    const auto time_now =
        std::chrono::system_clock::to_time_t(epoch += ms_since_epoch);

    std::stringstream ss;
    errno           = 0;
    auto local_time = std::localtime(&time_now);
    if (local_time == nullptr) {
        std::string err_str;
        if (errno > 0) {
            err_str = strerror(errno);
        }
        LOG_ERROR(
            "Failed to convert time to localtime: "
            << micros_to_string(ms_since_epoch) << " with error " << err_str);
        return {};
    }
    ss << std::put_time(local_time, "%Y%m%dT%H%M%SZ");
    return ss.str();
}

}  // namespace hestia
