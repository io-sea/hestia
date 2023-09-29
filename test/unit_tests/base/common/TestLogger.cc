#include <catch2/catch_all.hpp>

#include <filesystem>
#include <fstream>

#include "Logger.h"
#include "LoggerConfig.cc"

TEST_CASE("Test LoggerConfig Functions", "[common]")
{
    hestia::LoggerConfig logger_config;

    REQUIRE(!logger_config.modified());

    std::string path = "Log path";
    logger_config.set_log_path(path);
    auto result_path = logger_config.get_log_path();

    REQUIRE(result_path == path);

    std::string prefix = "Log prefix";
    logger_config.set_log_prefix(prefix);
    auto result_prefix = logger_config.get_log_prefix();

    REQUIRE(result_prefix == prefix);

    logger_config.set_console_only(true);
    REQUIRE(logger_config.is_console_only());

    logger_config.set_console_only(false);
    REQUIRE(!logger_config.is_console_only());

    hestia::LoggerConfig new_logger_config(logger_config);
    REQUIRE(logger_config.get_log_path() == new_logger_config.get_log_path());
    REQUIRE(
        logger_config.get_log_prefix() == new_logger_config.get_log_prefix());
    REQUIRE(logger_config.get_type() == new_logger_config.get_type());
    REQUIRE(logger_config.get_level() == new_logger_config.get_level());
    REQUIRE(logger_config.is_active() == new_logger_config.is_active());
    REQUIRE(
        logger_config.is_console_only() == new_logger_config.is_console_only());
    REQUIRE(logger_config.should_assert() == new_logger_config.should_assert());

    REQUIRE(logger_config.modified());

    logger_config.reset();
    REQUIRE(!logger_config.modified());
}