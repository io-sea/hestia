#include "Datamover.h"

#include "Logger.h"

#include <filesystem>

int main(int argc, char** argv)
{
    if (argc < 4) {
        std::cerr
            << "Expects at least 3 Args: <subject> <uuid> <command> [host]"
            << std::endl;
        return -1;
    }

    std::string subject = argv[1];
    std::string uuid    = argv[2];
    std::string command = argv[3];

    std::string host;
    if (argc == 5) {
        host = argv[4];
    }

    hestia::LoggerConfig logger_config;
    logger_config.set_active(true);
    logger_config.set_level(hestia::LoggerConfig::Level::DEBUG);
    logger_config.set_console_only(true);
    logger_config.set_should_assert_on_error(false);
    hestia::Logger::get_instance().do_initialize({}, logger_config);

    hestia::Datamover datamover(host);

    if (subject == "dataset") {
        datamover.apply_dataset_command(uuid, command);
    }
    else {
        datamover.apply_object_command(uuid, command);
    }
    return 0;
}