#include "EventFeed.h"

#include "Logger.h"
#include "Metadata.h"
#include "RbhEvent.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

namespace hestia {

void convert_put(
    const EventFeed::Event& event, std::string& str, const bool sorted = false);
void convert_remove(
    const EventFeed::Event& event, std::string& str, const bool sorted = false);
void convert_remove_all(
    const EventFeed::Event& event, std::string& str, const bool sorted = false);
void convert_copy(
    const EventFeed::Event& event, std::string& str, const bool sorted = false);
void convert_move(
    const EventFeed::Event& event, std::string& str, const bool sorted = false);

void event_to_string(
    const EventFeed::Event& event, std::string& str, const bool sorted = false);

void EventFeed::initialize(const EventFeedConfig& config)
{
    m_config = config;

    if (m_config.m_active == false) {
        return;
    }

    m_output_file.open(m_config.m_event_feed_file_path, std::ios::trunc);
    m_output_file.close();  // Flush output file
}

void EventFeed::log_event(const EventFeed::Event& event)
{
    if (!m_config.m_active) {
        return;
    }

    std::string str;
    event_to_string(event, str, m_config.m_sorted_keys);

    if (m_config.m_sorted_keys) {
        LOG_INFO("Serializing event in sorted order : Testing only")
    }
    LOG_INFO("Logging event to event feed");
    m_output_file.open(m_config.m_event_feed_file_path, std::ios::app);
    m_output_file << str;
    m_output_file.close();
}

void event_to_string(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    switch (event.m_method) {
        case EventFeed::Event::Method::PUT:
            return convert_put(event, str, sorted);
        case EventFeed::Event::Method::COPY:
            return convert_copy(event, str, sorted);
        case EventFeed::Event::Method::REMOVE_ALL:
            return convert_remove_all(event, str, sorted);
        case EventFeed::Event::Method::REMOVE:
            return convert_remove(event, str, sorted);
        case EventFeed::Event::Method::MOVE:
            return convert_move(event, str, sorted);
    }
}

void convert_put(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Metadata meta;  // TODO: Handle overwrite put as remove then put
    meta.set_item("id", event.m_id);
    meta.set_item("size", std::to_string(event.m_length));
    meta.set_item("tier", std::to_string(event.m_target_tier));

    RbhEvent(RbhEvent::RbhTypes::UPSERT, meta).to_string(str, sorted);
}

void convert_remove(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Metadata meta;  // TODO: Request better way of specifying delete tier
    meta.set_item("id", event.m_id + std::to_string(event.m_target_tier));

    RbhEvent(RbhEvent::RbhTypes::DELETE, meta).to_string(str, sorted);
}

void convert_remove_all(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Metadata meta;  // TODO: Implement when updated in HsmObject
    meta.set_item("id", event.m_id);

    RbhEvent(RbhEvent::RbhTypes::DELETE, meta).to_string(str, sorted);
}

void convert_copy(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Metadata meta;
    meta.set_item("id", event.m_id);
    meta.set_item("size", std::to_string(event.m_length));
    meta.set_item("source_tier", std::to_string(event.m_source_tier));
    meta.set_item("target_tier", std::to_string(event.m_target_tier));

    RbhEvent(RbhEvent::RbhTypes::UPSERT, meta).to_string(str, sorted);
}

void convert_move(
    const EventFeed::Event& event, std::string& str, const bool sorted)
{
    Metadata del_meta;
    std::string del_yaml, put_yaml;

    del_meta.set_item("id", event.m_id + std::to_string(event.m_source_tier));
    RbhEvent(RbhEvent::RbhTypes::DELETE, del_meta).to_string(del_yaml, sorted);

    convert_put(event, put_yaml, sorted);

    str = del_yaml + put_yaml;
}

}  // namespace hestia