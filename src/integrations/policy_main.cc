#include "PolicyEngine.h"

#include "Logger.h"

#include <chrono>
#include <filesystem>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Expects at least 2 Args: <command> <cache_dir> [host]"
                  << std::endl;
        return -1;
    }

    std::string command(argv[1]);
    if (command != "listen" && command != "query") {
        std::cerr << "Commands are 'listen' or 'query' - got: " << command
                  << std::endl;
        return -1;
    }

    std::string cache_path(argv[2]);
    auto cache_dir = std::filesystem::path(cache_path);
    if (cache_dir.is_relative()) {
        cache_dir = std::filesystem::current_path() / cache_dir;
    }

    std::string host;
    if (argc == 4) {
        host = argv[3];
    }

    hestia::LoggerConfig logger_config;
    logger_config.set_active(true);
    logger_config.set_level(hestia::LoggerConfig::Level::INFO);
    logger_config.set_console_only(true);
    logger_config.set_should_assert_on_error(false);
    hestia::Logger::get_instance().do_initialize({}, logger_config);

    if (command == "listen") {
        hestia::PolicyEngine policy_engine(cache_dir, host);
        policy_engine.initialize_db(true);

        std::chrono::microseconds sync_time{0};
        auto rc = policy_engine.do_sync(sync_time);
        if (rc != 0) {
            std::cerr << " Failed initial sync" << std::endl;
            return rc;
        }
        policy_engine.start_event_listener(sync_time);
    }
    else {
        hestia::PolicyEngine policy_engine(cache_dir, host);
        policy_engine.initialize_db();
        policy_engine.log_db();
    }

    return 0;
}