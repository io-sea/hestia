#pragma once

#include "TimedLock.h"

namespace hestia {
class Lockable {
  public:
    void read_lock();

    void write_lock();

    void read_unlock();

    void write_unlock();

    bool is_read_locked();

    bool is_write_locked();

  protected:
    TimedLock m_read_lock;
    TimedLock m_write_lock;
};
}  // namespace hestia