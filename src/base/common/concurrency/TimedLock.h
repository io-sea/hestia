#pragma once

#include "Dictionary.h"
#include "TimeUtils.h"

namespace hestia {

class TimedLock {
  public:
    TimedLock(std::time_t timeout = 5000);

    void deserialize(const Dictionary& dict);

    void lock();

    Dictionary::Ptr serialize() const;

    void unlock();

    bool m_active{false};
    std::time_t m_locked_at{0};
    std::time_t m_max_lock_time{5000};
};

};  // namespace hestia