#pragma once

#include "Model.h"
#include "TimedLock.h"

namespace hestia {
class LockableModel : public Model {
  public:
    LockableModel(const std::string& type);

    LockableModel(const std::string& id, const std::string& type);

    LockableModel(const LockableModel& other);

    virtual ~LockableModel();

    void read_lock();

    void write_lock();

    void read_unlock();

    void write_unlock();

    bool is_read_locked();

    bool is_write_locked();

    LockableModel& operator=(const LockableModel& other);

  private:
    void init();

    TypedDictField<TimedLock> m_read_lock{"read_lock"};
    TypedDictField<TimedLock> m_write_lock{"write_lock"};
};
}  // namespace hestia
