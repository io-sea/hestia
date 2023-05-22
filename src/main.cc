#include "ApplicationContext.h"
#include "HestiaCli.h"
#include "Logger.h"
#include "ProjectConfig.h"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv)
{
    hestia::HestiaCli hestia_cli;
    try {
        hestia_cli.parse(argc, argv);
    }
    catch (const std::exception& e) {
        return EXIT_FAILURE;
    }

    hestia::Logger::Config logger_config;
    logger_config.m_active       = true;
    logger_config.m_level        = hestia::Logger::Level::INFO;
    logger_config.m_console_only = false;
    logger_config.m_assert       = false;
    logger_config.m_log_prefix   = "hestia_cli";
    logger_config.m_log_file_path =
        std::filesystem::current_path() / "hestia_log.txt";

    hestia::Logger::get_instance().do_initialize(logger_config);

    LOG_INFO(
        "Starting Hestia Version: "
        << hestia::project_config::get_project_version());

    int rc = 0;
    try {
        const auto cli_status = hestia_cli.run();
        if (!cli_status.ok()) {
            rc = -1;
            std::cerr << cli_status.str() << "\n";
            std::cerr << "See " << logger_config.m_log_file_path
                      << " for details." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Uncaught exception running Hestia: " << e.what()
                  << std::endl;
        rc = -1;
    }

    try {
        hestia::ApplicationContext::get().clear();
    }
    catch (const std::exception& e) {
        std::cerr << "Uncaught exception clearing Hestia Context: " << e.what()
                  << std::endl;
        rc = -1;
    }

    LOG_INFO("Hestia Finished");
    return rc;
}