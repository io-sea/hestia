#include "PolicyEngine.h"

#include "Logger.h"

#include <filesystem>

int main(int argc, char** argv)
{
    std::filesystem::path cache_dir;
    if (argc == 2) {
        cache_dir = std::filesystem::path(argv[1]);
        if (cache_dir.is_relative()) {
            cache_dir = std::filesystem::current_path() / cache_dir;
        }
    }
    else {
        cache_dir = std::filesystem::current_path();
    }

    hestia::LoggerConfig logger_config;
    logger_config.set_active(true);
    logger_config.set_level(hestia::LoggerConfig::Level::INFO);
    logger_config.set_console_only(true);
    logger_config.set_should_assert_on_error(false);
    hestia::Logger::get_instance().do_initialize({}, logger_config);

    hestia::PolicyEngine policy_engine(cache_dir);
    policy_engine.initialize_db();
    policy_engine.do_initial_sync();
    policy_engine.start_event_listener();
    return 0;
}