#include "hestia.h"

#include "ApplicationContext.h"
#include "CommandLineArgs.h"
#include "HestiaCli.h"
#include "HestiaConfig.h"
#include "HsmService.h"

#include "Logger.h"

#include <iostream>

hestia::TierBackendRegistry g_all_file_backend_example = {
    {000, hestia::ObjectStoreClientType(
              hestia::ObjectStoreClientType::Type::HSM,
              hestia::ObjectStoreClientType::Source::BUILT_IN,
              "file")},
    {100, hestia::ObjectStoreClientType(
              hestia::ObjectStoreClientType::Type::HSM,
              hestia::ObjectStoreClientType::Source::BUILT_IN,
              "file")},
    {200, hestia::ObjectStoreClientType(
              hestia::ObjectStoreClientType::Type::HSM,
              hestia::ObjectStoreClientType::Source::BUILT_IN,
              "file")},
    {300, hestia::ObjectStoreClientType(
              hestia::ObjectStoreClientType::Type::HSM,
              hestia::ObjectStoreClientType::Source::BUILT_IN,
              "file")},
};

int main(int argc, char** argv)
{
    hestia::HestiaCli hestia_cli;
    hestia_cli.parse(argc, argv);

    hestia::HestiaConfig config;
    config.m_key_value_store_type  = hestia::KeyValueStoreType::FILE;
    config.m_placement_engine_type = PlacementEngineType::BASIC;

    hestia::Logger::Config logger_config;
    logger_config.m_active       = true;
    logger_config.m_level        = hestia::Logger::Level::INFO;
    logger_config.m_console_only = false;
    logger_config.m_log_prefix   = "hestia";
    logger_config.m_log_file_path =
        std::filesystem::current_path() / "hestia_log.txt";

    hestia::Logger::get_instance().initialize(logger_config);

    LOG_INFO("Starting Hestia");

    hestia::HestiaConfigurator configurator;
    auto result = configurator.initialize(config);
    if (result != 0) {
        LOG_ERROR("Error configuring Hestia");
        return -1;
    }

    if (hestia_cli.m_method == hestia::HestiaCli::Method::GET) {
        LOG_INFO("Handling GET CLI option");
        hestia::HsmServiceRequest request(
            hestia::StorageObject(hestia_cli.m_object_id),
            hestia::HsmServiceRequestMethod::GET);

        auto stream = hestia::Stream::create();
        auto response =
            hestia::ApplicationContext::get().get_hsm_service()->make_request(
                request, stream.get());

        if (!response->ok()) {
            LOG_ERROR("Failed GET request");
            return -1;
        }
    }

    LOG_INFO("Hestia Finished - OK");
    return 0;
}