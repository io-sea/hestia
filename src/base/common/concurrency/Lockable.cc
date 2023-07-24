#include "Lockable.h"

namespace hestia {
void Lockable::read_lock()
{
    m_read_lock.lock();
}

void Lockable::write_lock()
{
    m_write_lock.lock();
}

void Lockable::read_unlock()
{
    m_read_lock.unlock();
}

void Lockable::write_unlock()
{
    m_write_lock.unlock();
}

bool Lockable::is_read_locked()
{
    return m_read_lock.m_active;
}

bool Lockable::is_write_locked()
{
    return m_write_lock.m_active;
}
}  // namespace hestia