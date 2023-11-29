#include "EventFeed.h"

#include "EventSink.h"
#include "Logger.h"
#include "Map.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

namespace hestia {

EventFeedConfig::EventFeedConfig() : SerializeableWithFields(s_type)
{
    init();
}

EventFeedConfig::EventFeedConfig(const EventFeedConfig& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

std::string EventFeedConfig::get_type()
{
    return s_type;
}

EventFeedConfig& EventFeedConfig::operator=(const EventFeedConfig& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_output_path = other.m_output_path;
        m_active      = other.m_active;
        init();
    }
    return *this;
}

void EventFeedConfig::init()
{
    register_scalar_field(&m_output_path);
    register_scalar_field(&m_active);
}

bool EventFeedConfig::is_active() const
{
    return m_active.get_value();
}

void EventFeedConfig::set_is_active(bool is_active)
{
    m_active.update_value(is_active);
}

const std::string& EventFeedConfig::get_output_path() const
{
    return m_output_path.get_value();
}

void EventFeed::initialize(const EventFeedConfig& config)
{
    m_config = config;
}

void EventFeed::add_sink(std::unique_ptr<EventSink> sink)
{
    m_sinks.push_back(std::move(sink));
}

void EventFeed::on_event(const CrudEvent& event)
{
    if (!m_config.is_active()) {
        return;
    }

    for (const auto& sink : m_sinks) {
        sink->on_event(event);
    }
}

}  // namespace hestia