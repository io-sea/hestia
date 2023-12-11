#pragma once

#include "Serializeable.h"

#include <chrono>

namespace hestia {

class TimedLock : public Serializeable {
  public:
    TimedLock(std::chrono::seconds timeout = std::chrono::seconds(5000));

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

    void lock();

    void unlock();

    bool m_active{false};
    std::chrono::microseconds m_locked_at{0};
    std::chrono::seconds m_max_lock_duration{5000};
};

}  // namespace hestia
