#pragma once

#include "CrudEvent.h"
#include "EventSink.h"
#include "SerializeableWithFields.h"

namespace hestia {
class EventFeedConfig : public SerializeableWithFields {

  public:
    EventFeedConfig();

    EventFeedConfig(const EventFeedConfig& other);

    static std::string get_type();

    bool is_active() const;

    const std::string& get_output_path() const;

    EventFeedConfig& operator=(const EventFeedConfig& other);

  private:
    void init();

    static constexpr const char s_type[]{"event_feed"};
    StringField m_output_path{"output_path", "event_feed.yaml"};
    BooleanField m_active{"active", false};
};

/// Class for logging filesystem events to librobinhood-compatible YAML
class EventFeed {
  public:
    /// @brief Initialize an instance with the config provided
    /// @param config EventFeed configuration
    void initialize(const EventFeedConfig& config);

    void add_sink(std::unique_ptr<EventSink> sink);

    /// @brief Serialize a filesystem-impacting event
    /// @param req Event data to serialize
    void on_event(const CrudEvent& event);

  private:
    std::vector<std::unique_ptr<EventSink>> m_sinks;
    EventFeedConfig m_config;
};
}  // namespace hestia