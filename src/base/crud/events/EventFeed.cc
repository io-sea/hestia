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

const std::string& EventFeedConfig::get_output_path() const
{
    return m_output_path.get_value();
}

void EventFeed::initialize(const EventFeedConfig& config)
{
    m_config = config;

    if (m_config.is_active() == false) {
        return;
    }
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

/*
void convert_put(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Map meta;  // TODO: Handle overwrite put as remove then put
    meta.set_item("id", event.m_id + std::to_string(event.m_target_tier));
    meta.set_item("size", std::to_string(event.m_length));
    meta.set_item("tier", std::to_string(event.m_target_tier));

    RbhEvent(RbhEvent::RbhTypes::UPSERT, meta).to_string(str, sorted);
}

void convert_remove(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Map meta;  // TODO: Request better way of specifying delete tier
    meta.set_item("id", event.m_id + std::to_string(event.m_target_tier));

    RbhEvent(RbhEvent::RbhTypes::DELETE, meta).to_string(str, sorted);
}

void convert_remove_all(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Map meta;  // TODO: Implement when updated in HsmObject
    meta.set_item("id", event.m_id);

    RbhEvent(RbhEvent::RbhTypes::DELETE, meta).to_string(str, sorted);
}

void convert_copy(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Map meta;
    meta.set_item("id", event.m_id + std::to_string(event.m_target_tier));
    meta.set_item("size", std::to_string(event.m_length));
    meta.set_item("source_tier", std::to_string(event.m_source_tier));
    meta.set_item("target_tier", std::to_string(event.m_target_tier));

    RbhEvent(RbhEvent::RbhTypes::UPSERT, meta).to_string(str, sorted);
}

void convert_move(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Map del_meta;
    std::string del_yaml, put_yaml;

    del_meta.set_item("id", event.m_id + std::to_string(event.m_source_tier));
    RbhEvent(RbhEvent::RbhTypes::DELETE, del_meta).to_string(del_yaml, sorted);

    convert_put(event, put_yaml, sorted);

    str = del_yaml + put_yaml;
}
*/

}  // namespace hestia