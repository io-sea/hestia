#include <catch2/catch_session.hpp>

#include "ApplicationContext.h"
#include "EventFeed.h"
#include "Logger.h"

#include <filesystem>

int main(int argc, char* argv[])
{
    hestia::Logger::Config logger_config;
    logger_config.m_active       = false;  // Can toggle when debugging tests.
    logger_config.m_level        = hestia::Logger::Level::INFO;
    logger_config.m_console_only = true;
    hestia::Logger::get_instance().do_initialize(logger_config);

    if (!std::filesystem::exists(
            std::filesystem::current_path() / "test_output")) {
        std::filesystem::create_directories(
            std::filesystem::current_path() / "test_output");
    }

    int result = Catch::Session().run(argc, argv);

    return result;
}
