#include "LockableModel.h"

#include "Logger.h"

namespace hestia {
LockableModel::LockableModel(const std::string& type) : Model(type)
{
    init();
}

LockableModel::LockableModel(const std::string& id, const std::string& type) :
    Model(id, type)
{
    init();
}

LockableModel::LockableModel(const LockableModel& other) : Model(other)
{
    *this = other;
}

LockableModel::~LockableModel() {}

LockableModel& LockableModel::operator=(const LockableModel& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_read_lock  = other.m_read_lock;
        m_write_lock = other.m_write_lock;
        init();
    }
    return *this;
}

void LockableModel::init()
{
    register_map_field(&m_read_lock);
    register_map_field(&m_write_lock);
}

void LockableModel::read_lock()
{
    m_read_lock.get_value_as_writeable().lock();
}

void LockableModel::write_lock()
{
    m_write_lock.get_value_as_writeable().lock();
}

void LockableModel::read_unlock()
{
    m_read_lock.get_value_as_writeable().unlock();
}

void LockableModel::write_unlock()
{
    m_write_lock.get_value_as_writeable().unlock();
}

bool LockableModel::is_read_locked()
{
    return m_read_lock.value().m_active;
}

bool LockableModel::is_write_locked()
{
    return m_write_lock.value().m_active;
}

}  // namespace hestia
