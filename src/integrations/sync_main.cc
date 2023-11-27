#include "SyncTool.h"

#include "Logger.h"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 4) {
        std::cerr << "Expects 3 Args: <command> <work_dir> <dataset_name>"
                  << std::endl;
        return -1;
    }

    std::string command(argv[1]);
    if (command != "archive" && command != "restore") {
        std::cerr << "Command is 'archive' or 'restore' - got: " << command
                  << std::endl;
        return -1;
    }

    std::string sync_dir_str(argv[2]);
    auto sync_dir = std::filesystem::path(sync_dir_str);
    if (sync_dir.is_relative()) {
        sync_dir = std::filesystem::current_path() / sync_dir;
    }
    if (!std::filesystem::is_directory(sync_dir)) {
        std::cerr << "Expects existing directory for 'work_dir'" << std::endl;
    }

    std::string dataset_name(argv[3]);

    hestia::LoggerConfig logger_config;
    logger_config.set_active(true);
    logger_config.set_level(hestia::LoggerConfig::Level::INFO);
    logger_config.set_console_only(true);
    logger_config.set_should_assert_on_error(false);
    hestia::Logger::get_instance().do_initialize({}, logger_config);

    hestia::SyncTool sync_tool(sync_dir);

    if (command == "archive") {
        sync_tool.archive(dataset_name);
    }
    else {
        sync_tool.restore(dataset_name);
    }

    return 0;
}