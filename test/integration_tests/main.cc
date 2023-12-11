#include <catch2/catch_session.hpp>

#include "EventFeed.h"
#include "Logger.h"

#include <filesystem>

int main(int argc, char* argv[])
{
    hestia::LoggerConfig logger_config;
    logger_config.set_active(false);  // Can toggle when debugging tests.
    logger_config.set_level(hestia::LoggerConfig::Level::DEBUG);
    logger_config.set_console_only(true);
    logger_config.set_should_assert_on_error(true);
    hestia::Logger::get_instance().do_initialize({}, logger_config);

    if (!std::filesystem::exists(
            std::filesystem::current_path() / "test_output")) {
        std::filesystem::create_directories(
            std::filesystem::current_path() / "test_output");
    }

    int result = Catch::Session().run(argc, argv);

    return result;
}
