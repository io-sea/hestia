#include "Logger.h"

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

void Logger::initialize(const Metadata& config_data)
{
    Config config;
    auto on_item = [&config](const std::string& key, const std::string& value) {
        if (key == "path") {
            config.m_log_file_path = value;
        }
        else if (key == "prefix") {
            config.m_log_prefix = std::stoi(value);
        }
        else if (key == "level") {
            if (value == "INFO") {
                config.m_level = Logger::Level::INFO;
            }
            else if (value == "WARN") {
                config.m_level = Logger::Level::WARN;
            }
            else if (value == "DEBUG") {
                config.m_level = Logger::Level::DEBUG;
            }
            else if (value == "ERROR") {
                config.m_level = Logger::Level::ERROR;
            }
        }
    };
    config_data.for_each_item(on_item);
    do_initialize(config);
}

void Logger::initialize(const LoggerContext& context)
{
    if (context.m_logger_impl) {
        spdlog::set_default_logger(context.m_logger_impl);
    }
}

const LoggerContext& Logger::get_context() const
{
    return m_context;
}

void Logger::do_initialize(const Config& config)
{
    m_config = config;

    if (m_config.m_active == false) {
        return;
    }

    if (!m_config.m_console_only) {
        auto logger = spdlog::basic_logger_mt(
            m_config.m_log_prefix, m_config.m_log_file_path, true);
        m_context.m_logger_impl = logger;

        spdlog::set_default_logger(logger);
    }
    switch (m_config.m_level) {
        case Level::ERROR:
            spdlog::set_level(spdlog::level::err);
            break;
        case Level::WARN:
            spdlog::set_level(spdlog::level::warn);
            break;
        case Level::INFO:
            spdlog::set_level(spdlog::level::info);
            break;
        case Level::DEBUG:
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

void Logger::log_line(
    Level level,
    const std::ostringstream& line,
    const std::string& file_name,
    const std::string& function_name,
    int line_number)
{
    if (!m_config.m_active) {
        return;
    }

    std::stringstream sstr;
    sstr << "[" << std::this_thread::get_id() << "] "
         << location_prefix(file_name, function_name, line_number) << "| "
         << line.str();

    switch (level) {
        case Level::ERROR:
            spdlog::error(sstr.str());
            break;
        case Level::WARN:
            spdlog::warn(sstr.str());
            break;
        case Level::INFO:
            spdlog::info(sstr.str());
            break;
        case Level::DEBUG:
            spdlog::debug(sstr.str());
            break;
        default:
            spdlog::debug(sstr.str());
            break;
    }

    if (m_config.m_assert && (level == Level::WARN || level == Level::ERROR)) {
        assert(false);
    }
}

}  // namespace hestia
