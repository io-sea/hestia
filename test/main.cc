#include <catch2/catch_session.hpp>

#include "Logger.h"

int main(int argc, char* argv[])
{
    hestia::Logger::Config logger_config;
    logger_config.m_active       = false;  // Can toggle when debugging tests.
    logger_config.m_level        = hestia::Logger::Level::WARN;
    logger_config.m_console_only = true;

    hestia::Logger::get_instance().initialize(logger_config);

    int result = Catch::Session().run(argc, argv);

    return result;
}