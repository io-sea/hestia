#include "hestia.h"

#include "HestiaCli.h"
#include "Logger.h"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv)
{
    hestia::HestiaCli hestia_cli;
    try {
        hestia_cli.parse(argc, argv);
    }
    catch (const std::exception& ex) {
        return EXIT_FAILURE;
    }

    hestia::Logger::Config logger_config;
    logger_config.m_active       = true;
    logger_config.m_level        = hestia::Logger::Level::INFO;
    logger_config.m_console_only = false;
    logger_config.m_log_prefix   = "hestia";
    logger_config.m_log_file_path =
        std::filesystem::current_path() / "hestia_log.txt";

    hestia::Logger::get_instance().initialize(logger_config);

    LOG_INFO("Starting Hestia");

    const auto rc = hestia_cli.run();

    LOG_INFO("Hestia Finished");
    return rc;
}