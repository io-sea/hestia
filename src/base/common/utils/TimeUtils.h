#pragma once

#include <chrono>
#include <string>

namespace hestia {
class TimeUtils {
  public:
    static std::chrono::microseconds get_time_since_epoch_micros();
    static std::string get_time_since_epoch_micross_str();

    static std::chrono::microseconds micros_from_string(const std::string& in);

    static std::string micros_to_string(std::chrono::microseconds ms);

    static std::string get_current_time_readable();
    static std::string get_current_time_iso8601_basic();
    static std::string to_iso8601_basic(
        std::chrono::microseconds micros_since_epoch);
};
}  // namespace hestia
