#pragma once

#include "TimeProvider.h"

namespace hestia::mock {

class MockTimeProvider : public TimeProvider {
  public:
    std::time_t get_current_time() const override { return m_current_time; }

    void increment(unsigned step = 1) { m_current_time += step; }

    std::time_t m_current_time{0};
};
}  // namespace hestia::mock