#pragma once

#include "LoggerConfig.h"

#include <memory>
#include <sstream>

#define LOG_BASE(msg, level)                                                   \
    {                                                                          \
        std::ostringstream logstream;                                          \
        logstream << msg;                                                      \
        hestia::Logger::get_instance().log_line(                               \
            level, logstream, __FILE__, __FUNCTION__, __LINE__);               \
    };

#define LOG_ERROR(msg) LOG_BASE(msg, hestia::LoggerConfig::Level::ERROR);
#define LOG_WARN(msg) LOG_BASE(msg, hestia::LoggerConfig::Level::WARN);
#define LOG_INFO(msg) LOG_BASE(msg, hestia::LoggerConfig::Level::INFO);
#define LOG_DEBUG(msg) LOG_BASE(msg, hestia::LoggerConfig::Level::DEBUG);

namespace spdlog {
class logger;
}

namespace hestia {

class LoggerContext {
  public:
    LoggerConfig m_config;
    std::shared_ptr<spdlog::logger> m_logger_impl;
};

class Logger {
  public:
    void initialize(const LoggerContext& context);

    void do_initialize(
        const std::string& cache_path, const LoggerConfig& config);

    static Logger& get_instance();

    const LoggerContext& get_context() const;

    LoggerContext& get_modifiable_context();

    void log_line(
        LoggerConfig::Level level,
        const std::ostringstream& line,
        const std::string& file_name     = "",
        const std::string& function_name = "",
        int line_number                  = -1);

  private:
    LoggerContext m_context;
};
}  // namespace hestia