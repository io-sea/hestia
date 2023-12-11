#pragma once

#include <chrono>

namespace hestia {

class TimeProvider {
  public:
    virtual ~TimeProvider()                                    = default;
    virtual std::chrono::microseconds get_current_time() const = 0;
};

class DefaultTimeProvider : public TimeProvider {
  public:
    std::chrono::microseconds get_current_time() const override;
};

}  // namespace hestia