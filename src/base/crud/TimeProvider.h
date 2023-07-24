#pragma once

#include <ctime>

namespace hestia {

class TimeProvider {
  public:
    virtual ~TimeProvider()                      = default;
    virtual std::time_t get_current_time() const = 0;
};

class DefaultTimeProvider : public TimeProvider {
  public:
    std::time_t get_current_time() const override;
};

}  // namespace hestia