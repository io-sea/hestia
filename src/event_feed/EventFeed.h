#pragma once

#include <filesystem>
#include <fstream>

namespace hestia {

struct EventFeedConfig {
    std::string m_event_feed_file_path{"event_feed.yaml"};
    bool m_active{true};
    bool m_sorted_keys{false};  // For debug purposes only
};

/// Class for logging filesystem events to librobinhood-compatible YAML
class EventFeed {
  public:
    struct Event {
        enum class Method { PUT, REMOVE, REMOVE_ALL, COPY, MOVE };

        std::string m_id;
        unsigned long m_length;

        unsigned int m_source_tier;
        unsigned int m_target_tier;

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