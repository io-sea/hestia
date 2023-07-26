#include "HestiaCli.h"
#include "HestiaClient.h"
#include "HestiaServer.h"

#include <iostream>

int main(int argc, char** argv)
{
    hestia::HestiaCli hestia_cli;
    try {
        hestia_cli.parse_args(argc, argv);
    }
    catch (const std::exception& e) {
        return EXIT_FAILURE;
    }

    std::unique_ptr<hestia::HestiaApplication> hestia_app;
    if (hestia_cli.is_client()) {
        hestia_app = std::make_unique<hestia::HestiaClient>();
    }
    else if (hestia_cli.is_server()) {
        hestia_app = std::make_unique<hestia::HestiaServer>();
    }

    int rc = 0;
    try {
        const auto cli_status = hestia_cli.run(hestia_app.get());
        if (!cli_status.ok()) {
            rc = -1;
            std::cerr << cli_status.str() << "\n";
            // std::cerr << "See " << logger_config.m_log_file_path
            //<< " for details." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Uncaught exception running Hestia: " << e.what()
                  << std::endl;
        rc = -1;
    }

    if (hestia_app) {
        try {
            hestia_app.reset();
        }
        catch (const std::exception& e) {
            std::cerr << "Uncaught exception clearing Hestia Context: "
                      << e.what() << std::endl;
            rc = -1;
        }
        LOG_INFO("Hestia Finished");
    }
    return rc;
}