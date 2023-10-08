#pragma once

#include <ctime>
#include <string>

namespace hestia {
class TimeUtils {
  public:
    static std::time_t get_current_time();
    static std::string get_current_time_hr();
    static std::string get_current_time_iso8601_basic();
    static std::string to_iso8601_basic(std::time_t time);
};
}  // namespace hestia
