#pragma once

#include <ctime>

namespace hestia {
class TimeUtils {
  public:
    static std::time_t get_current_time();
};
}  // namespace hestia
