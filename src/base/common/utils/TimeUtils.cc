#include "TimeUtils.h"

#include <chrono>

namespace hestia {
std::time_t TimeUtils::get_current_time()
{
    return std::chrono::system_clock::now().time_since_epoch().count();
}
};  // namespace hestia