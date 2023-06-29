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
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%F_%H-%M-%S");
    return ss.str();
}
};  // namespace hestia