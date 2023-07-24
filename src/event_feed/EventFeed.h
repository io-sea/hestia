#pragma once

#include "SerializeableWithFields.h"

#include <filesystem>
#include <fstream>

namespace hestia {

class EventFeedConfig : public SerializeableWithFields {

  public:
    EventFeedConfig();

    EventFeedConfig(const EventFeedConfig& other);

    static std::string get_type();

    bool is_active() const;

    const std::string& get_output_path() const;

    bool should_sort_keys() const;

    EventFeedConfig& operator=(const EventFeedConfig& other);

  private:
    void init();

    static constexpr const char s_type[]{"event_feed"};
    StringField m_output_path{"output_path", "event_feed.yaml"};
    BooleanField m_active{"active", true};
    BooleanField m_sorted_keys{
        "sorted_keys", false};  // For debug purposes only
};

/// Class for logging filesystem events to librobinhood-compatible YAML
class EventFeed {
  public:
    struct Event {
        enum class Method { PUT, REMOVE, REMOVE_ALL, COPY, MOVE };

        std::string m_id;
        unsigned long m_length{0};

        unsigned int m_source_tier{0};
        unsigned int m_target_tier{0};

        Method m_method;
        void method_to_string(std::string& out);
    };

    /// @brief Initialize an instance with the config provided
    /// @param config EventFeed configuration
    void initialize(
        const std::string& cache_path, const EventFeedConfig& config);

    /// @brief Serialize a filesystem-impacting event
    /// @param req Event data to serialize
    void log_event(const Event& event);

  private:
    EventFeedConfig m_config;
    std::filesystem::path m_output_file_path;
    std::ofstream m_output_file;
};
}  // namespace hestia