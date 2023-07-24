#include "Logger.h"

#include "TimeUtils.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>
#include <time.h>

namespace hestia {

Logger& Logger::get_instance()
{
    static Logger instance;
    return instance;
}

const LoggerContext& Logger::get_context() const
{
    return m_context;
}

void Logger::initialize(const LoggerContext& context)
{
    m_context.m_config = context.m_config;

    if (context.m_logger_impl) {
        spdlog::set_default_logger(context.m_logger_impl);
    }
}

void Logger::do_initialize(
    const std::string& cache_path, const LoggerConfig& config)
{
    m_context.m_config = config;

    if (!m_context.m_config.is_active()) {
        return;
    }

    if (!m_context.m_config.is_console_only()) {
        if (m_context.m_config.get_log_prefix().empty()) {
            m_context.m_config.set_log_prefix("hestia_app");
        }

        if (m_context.m_config.get_log_path().empty()) {
            m_context.m_config.set_log_path("hestia_log");
        }

        auto logger_path =
            std::filesystem::path(m_context.m_config.get_log_path());
        if (logger_path.is_relative()) {
            logger_path = std::filesystem::path(cache_path) / logger_path;
        }
        const std::string suffix =
            "_" + TimeUtils::get_current_time_hr() + ".txt";
        logger_path += suffix;

        auto logger = spdlog::basic_logger_mt(
            m_context.m_config.get_log_prefix(), logger_path.string(), true);
        m_context.m_logger_impl = logger;

        spdlog::set_default_logger(logger);
    }
    switch (m_context.m_config.get_level()) {
        case LoggerConfig::Level::ERROR:
            spdlog::set_level(spdlog::level::err);
            break;
        case LoggerConfig::Level::WARN:
            spdlog::set_level(spdlog::level::warn);
            break;
        case LoggerConfig::Level::INFO:
            spdlog::set_level(spdlog::level::info);
            break;
        case LoggerConfig::Level::DEBUG:
            spdlog::set_level(spdlog::level::debug);
            break;
        default:
            spdlog::set_level(spdlog::level::debug);
            break;
    }

    spdlog::flush_on(spdlog::level::info);
}

std::string location_prefix(
    const std::string& file_name,
    const std::string& function_name,
    int line_number)
{
    return std::filesystem::path(file_name).stem().string()
           + "::" + function_name + "::" + std::to_string(line_number);
}

LoggerContext& Logger::get_modifiable_context()
{
    return m_context;
}

void Logger::log_line(
    LoggerConfig::Level level,
    const std::ostringstream& line,
    const std::string& file_name,
    const std::string& function_name,
    int line_number)
{
    if (!m_context.m_config.is_active()) {
        return;
    }

    std::stringstream sstr;
    sstr << "[" << std::this_thread::get_id() << "] "
         << location_prefix(file_name, function_name, line_number) << "| "
         << line.str();

    switch (level) {
        case LoggerConfig::Level::ERROR:
            spdlog::error(sstr.str());
            break;
        case LoggerConfig::Level::WARN:
            spdlog::warn(sstr.str());
            break;
        case LoggerConfig::Level::INFO:
            spdlog::info(sstr.str());
            break;
        case LoggerConfig::Level::DEBUG:
            spdlog::debug(sstr.str());
            break;
        default:
            spdlog::debug(sstr.str());
            break;
    }

    if (m_context.m_config.should_assert()
        && (level == LoggerConfig::Level::WARN
            || level == LoggerConfig::Level::ERROR)) {
        assert(false);
    }
}

}  // namespace hestia
