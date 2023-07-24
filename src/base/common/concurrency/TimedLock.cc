#include "TimedLock.h"

#include "TimeUtils.h"

namespace hestia {
TimedLock::TimedLock(std::time_t timeout) :
    Serializeable("timed_lock"), m_max_lock_time(timeout)
{
}

void TimedLock::lock()
{
    m_active    = true;
    m_locked_at = TimeUtils::get_current_time();
}

void TimedLock::unlock()
{
    m_active    = false;
    m_locked_at = 0;
}

void TimedLock::serialize(Dictionary& dict, Format) const
{
    std::unordered_map<std::string, std::string> data = {
        {"active", m_active ? "true" : "false"},
        {"locked_at", std::to_string(m_locked_at)},
        {"max_lock_time", std::to_string(m_max_lock_time)},
    };
    dict.set_map(data);
}

void TimedLock::deserialize(const Dictionary& dict, Format)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "active" && !value.empty()) {
            m_active = (key == "true");
        }
        else if (key == "locked_at" && !value.empty()) {
            m_locked_at = std::stoull(value);
        }
        else if (key == "max_lock_time" && !value.empty()) {
            m_max_lock_time = std::stoull(value);
        }
    };
    dict.for_each_scalar(on_item);
}
}  // namespace hestia
