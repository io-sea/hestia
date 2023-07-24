#pragma once

#include "Serializeable.h"

#include <ctime>

namespace hestia {

class TimedLock : public Serializeable {
  public:
    TimedLock(std::time_t timeout = 5000);

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

    void lock();

    void unlock();

    bool m_active{false};
    std::time_t m_locked_at{0};
    std::time_t m_max_lock_time{5000};
};

}  // namespace hestia
