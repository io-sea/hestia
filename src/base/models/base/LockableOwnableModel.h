#pragma once

#include "OwnableModel.h"
#include "TimedLock.h"

namespace hestia {
class LockableOwnableModel : public OwnableModel {
  public:
    LockableOwnableModel(
        const std::string& type, const std::string& created_by);

    LockableOwnableModel(
        const std::string& id,
        const std::string& type,
        const std::string& created_by);

    LockableOwnableModel(const LockableOwnableModel& other);

    void read_lock();

    void write_lock();

    void read_unlock();

    void write_unlock();

    bool is_read_locked();

    bool is_write_locked();

    LockableOwnableModel& operator=(const LockableOwnableModel& other);

  private:
    void init();

    TypedDictField<TimedLock> m_read_lock{"read_lock"};
    TypedDictField<TimedLock> m_write_lock{"write_lock"};
};
}  // namespace hestia
