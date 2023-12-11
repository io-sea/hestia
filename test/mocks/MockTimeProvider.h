#pragma once

#include "TimeProvider.h"

namespace hestia::mock {

class MockTimeProvider : public TimeProvider {
  public:
    std::chrono::microseconds get_current_time() const override
    {
        return m_current_time;
    }

    void increment(
        std::chrono::microseconds step = std::chrono::microseconds(1))
    {
        m_current_time += step;
    }

    std::chrono::microseconds m_current_time{0};
};
}  // namespace hestia::mock