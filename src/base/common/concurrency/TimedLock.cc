#include "TimedLock.h"

#include "TimeUtils.h"

namespace hestia {
TimedLock::TimedLock(std::chrono::seconds timeout) :
    Serializeable("timed_lock"), m_max_lock_duration(timeout)
{
}

void TimedLock::lock()
{
    m_active    = true;
    m_locked_at = TimeUtils::get_time_since_epoch_micros();
}

void TimedLock::unlock()
{
    m_active    = false;
    m_locked_at = std::chrono::microseconds(0);
}

void TimedLock::serialize(Dictionary& dict, Format) const
{
    dict.set_map_scalar(
        "active", m_active ? "true" : "false", Dictionary::ScalarType::BOOL);
    dict.set_map_scalar(
        "locked_at", std::to_string(m_locked_at.count()),
        Dictionary::ScalarType::INT);
    dict.set_map_scalar(
        "max_lock_time", std::to_string(m_max_lock_duration.count()),
        Dictionary::ScalarType::INT);
}

void TimedLock::deserialize(const Dictionary& dict, Format)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "active" && !value.empty()) {
            m_active = (key == "true");
        }
        else if (key == "locked_at" && !value.empty()) {
            m_locked_at = std::chrono::microseconds(std::stoull(value));
        }
        else if (key == "max_lock_time" && !value.empty()) {
            m_max_lock_duration = std::chrono::seconds(std::stoull(value));
        }
    };
    dict.for_each_scalar(on_item);
}
}  // namespace hestia
