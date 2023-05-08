#include "hestia.h"

#include "ApplicationContext.h"
#include "CommandLineArgs.h"
#include "FileStreamSink.h"
#include "FileStreamSource.h"
#include "HestiaCli.h"
#include "HestiaConfig.h"
#include "HsmService.h"

#include "Logger.h"

#include <filesystem>
#include <iostream>

hestia::TierBackendRegistry g_all_file_backend_example = {
    {000, hestia::ObjectStoreClientType(
              hestia::ObjectStoreClientType::Type::HSM,
              hestia::ObjectStoreClientType::Source::BUILT_IN,
              "hestia::FileHsmObjectStoreClient")},
    {100, hestia::ObjectStoreClientType(
              hestia::ObjectStoreClientType::Type::HSM,
              hestia::ObjectStoreClientType::Source::BUILT_IN,
              "hestia::FileHsmObjectStoreClient")},
    {200, hestia::ObjectStoreClientType(
              hestia::ObjectStoreClientType::Type::HSM,
              hestia::ObjectStoreClientType::Source::BUILT_IN,
              "hestia::FileHsmObjectStoreClient")},
    {300, hestia::ObjectStoreClientType(
              hestia::ObjectStoreClientType::Type::HSM,
              hestia::ObjectStoreClientType::Source::BUILT_IN,
              "hestia::FileHsmObjectStoreClient")},
};

int main(int argc, char** argv)
{
    hestia::HestiaCli hestia_cli;
    try {
        hestia_cli.parse(argc, argv);
    }
    catch (const std::exception& ex) {
        return EXIT_FAILURE;
    }

    hestia::HestiaConfig config;
    config.m_key_value_store_type  = hestia::KeyValueStoreType::FILE;
    config.m_placement_engine_type = hestia::PlacementEngineType::BASIC;
    config.m_tier_backend_registry = g_all_file_backend_example;

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
        std::cerr << "Error configuring Hestia" << std::endl;
        LOG_ERROR("Error configuring Hestia");
        return -1;
    }

    hestia::HsmServiceRequest::Ptr request;
    hestia::Stream::Ptr stream;

    if (hestia_cli.m_method == hestia::HestiaCli::Method::GET) {
        LOG_INFO("Handling GET CLI option - object: " + hestia_cli.m_object_id);
        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(hestia_cli.m_object_id),
            hestia::HsmServiceRequestMethod::GET);
        request->set_target_tier(hestia_cli.m_target_tier);

        stream    = hestia::Stream::create();
        auto sink = hestia::FileStreamSink::create(hestia_cli.m_path);
        stream->set_sink(std::move(sink));
    }
    else if (hestia_cli.m_method == hestia::HestiaCli::Method::PUT) {
        LOG_INFO("Handling PUT CLI option - object: " + hestia_cli.m_object_id);

        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(hestia_cli.m_object_id),
            hestia::HsmServiceRequestMethod::PUT);
        request->set_source_tier(hestia_cli.m_source_tier);

        stream      = hestia::Stream::create();
        auto source = hestia::FileStreamSource::create(hestia_cli.m_path);
        stream->set_source(std::move(source));
    }

    if (request) {
        auto response =
            hestia::ApplicationContext::get().get_hsm_service()->make_request(
                *(request.get()), stream.get());

        if (!response->ok()) {
            std::cerr << "Object Store request failed with: "
                      << response->get_error().to_string() << std::endl;
            LOG_ERROR("Request Failed");
            return -1;
        }
    }

    if (stream) {
        auto stream_state = stream->flush();
        if (!stream_state.ok()) {
            std::cerr << "Object Store stream failed with: "
                      << stream_state.to_string() << std::endl;
            LOG_ERROR("Stream Failed");
            return -1;
        }
    }

    LOG_INFO("Hestia Finished - OK");
    return 0;
}