#include "LockableOwnableModel.h"

#include "Logger.h"

namespace hestia {
LockableOwnableModel::LockableOwnableModel(
    const std::string& type, const std::string& created_by) :
    OwnableModel(type, created_by)
{
    init();
}

LockableOwnableModel::LockableOwnableModel(
    const std::string& id,
    const std::string& type,
    const std::string& created_by) :
    OwnableModel(id, type, created_by)
{
    init();
}

LockableOwnableModel::LockableOwnableModel(const LockableOwnableModel& other) :
    OwnableModel(other)
{
    *this = other;
}

LockableOwnableModel& LockableOwnableModel::operator=(
    const LockableOwnableModel& other)
{
    if (this != &other) {
        OwnableModel::operator=(other);
        m_read_lock  = other.m_read_lock;
        m_write_lock = other.m_write_lock;
        init();
    }
    return *this;
}

void LockableOwnableModel::init()
{
    register_map_field(&m_read_lock);
    register_map_field(&m_write_lock);
}

void LockableOwnableModel::read_lock()
{
    m_read_lock.get_value_as_writeable().lock();
}

void LockableOwnableModel::write_lock()
{
    m_write_lock.get_value_as_writeable().lock();
}

void LockableOwnableModel::read_unlock()
{
    m_read_lock.get_value_as_writeable().unlock();
}

void LockableOwnableModel::write_unlock()
{
    m_write_lock.get_value_as_writeable().unlock();
}

bool LockableOwnableModel::is_read_locked()
{
    return m_read_lock.value().m_active;
}

bool LockableOwnableModel::is_write_locked()
{
    return m_write_lock.value().m_active;
}

}  // namespace hestia
