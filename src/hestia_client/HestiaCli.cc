#include "HestiaCli.h"

#include "ApplicationContext.h"
#include "FileStreamSink.h"
#include "FileStreamSource.h"
#include "HestiaConfigurator.h"

#include "HestiaService.h"
#include "HsmS3Service.h"
#include "HsmService.h"

#include "S3Service.h"
#include "S3WebApp.h"
#include "Server.h"

#include "BasicHttpServer.h"
#include "CopyToolWebApp.h"
#include "CurlClient.h"
#include "FileKeyValueStoreClient.h"
#include "HestiaS3WebApp.h"
#include "HestiaWebApp.h"

#include "Logger.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <CLI/CLI11.hpp>

namespace hestia {

void HestiaCli::add_get_options(CLI::App* command)
{
    command->add_option("object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command
        ->add_option(
            "file", m_hsm_command.m_path, "Path to write object data to")
        ->required();
    command->add_option("source", m_hsm_command.m_source_tier, "Source Tier");
}

void HestiaCli::add_put_options(CLI::App* command)
{
    command->add_option("object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command->add_option("file", m_hsm_command.m_path, "Path to object data")
        ->required();
    command->add_option("target", m_hsm_command.m_target_tier, "Target Tier");
}

void HestiaCli::add_copy_options(CLI::App* command)
{
    command->add_option("object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command->add_option("source", m_hsm_command.m_source_tier, "Source Tier")
        ->required();
    command->add_option("target", m_hsm_command.m_target_tier, "Target Tier")
        ->required();
}

void HestiaCli::add_move_options(CLI::App* command)
{
    command->add_option("object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command->add_option("source", m_hsm_command.m_source_tier, "Source Tier")
        ->required();
    command->add_option("target", m_hsm_command.m_target_tier, "Target Tier")
        ->required();
}

void HestiaCli::add_release_options(CLI::App* command)
{
    command->add_option("object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command->add_option("source", m_hsm_command.m_source_tier, "Source Tier")
        ->required();
}

void HestiaCli::add_get_metadata_options(CLI::App* command)
{
    command->add_option("object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
}

void HestiaCli::add_put_metadata_options(CLI::App* command)
{
    command->add_option("object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command
        ->add_option(
            "attrs", m_hsm_command.m_attributes,
            "Attributes, format: {key0: val0; key1: val1}")
        ->required();
}

void HestiaCli::add_list_options(CLI::App* command)
{
    (void)command;
}

void HestiaCli::add_list_tiers_options(CLI::App* command)
{
    (void)command;
}

void HestiaCli::parse(int argc, char* argv[])
{
    CLI::App app{
        "Hestia - Hierarchical Storage Tiers Interface for Applications",
        "hestia"};

    std::unordered_map<std::string, CLI::App*> commands;
    commands["get"] = app.add_subcommand("get", "Get an object from the store");
    commands["put"] = app.add_subcommand("put", "Put an object to the store");
    commands["copy"] =
        app.add_subcommand("copy", "Copy an object between tiers");
    commands["move"] =
        app.add_subcommand("move", "Move an object between tiers");
    commands["release"] =
        app.add_subcommand("release", "Release an object from a tier");

    commands["put_attrs"] =
        app.add_subcommand("put_attrs", "Add attributes to an object.");
    commands["get_attrs"] =
        app.add_subcommand("get_attrs", "Get object attributes");
    commands["list"]       = app.add_subcommand("list", "List objects");
    commands["list_tiers"] = app.add_subcommand("list_tiers", "List tiers");

    commands["start"]  = app.add_subcommand("start", "Start the Hestia Daemon");
    commands["stop"]   = app.add_subcommand("stop", "Stop the Hestia Daemon");
    commands["server"] = app.add_subcommand("server", "Run the Hestia Server");

    add_get_options(commands["get"]);
    add_put_options(commands["put"]);
    add_copy_options(commands["copy"]);
    add_move_options(commands["move"]);
    add_release_options(commands["release"]);

    add_put_metadata_options(commands["put_attrs"]);
    add_get_metadata_options(commands["get_attrs"]);

    commands["server"]->add_option(
        "--host", m_config.m_server_config.m_host,
        "Hestia server host address");
    commands["server"]->add_option(
        "-p, --port", m_config.m_server_config.m_port,
        "Hestia server host port");

    for (auto& kv_pair : commands) {
        kv_pair.second->add_option(
            "-c, --config", m_config_path, "Path to a Hestia config file.");
    }

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        app.exit(e);
        throw std::runtime_error(
            "Returning after command line parsing. --help or invalid argument(s)");
    }

    if (commands["put"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::PUT;
    }
    else if (commands["get"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::GET;
    }
    else if (commands["copy"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::COPY;
    }
    else if (commands["move"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::MOVE;
    }
    else if (commands["release"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::RELEASE;
    }
    else if (commands["get_attrs"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::GET_METADATA;
    }
    else if (commands["put_attrs"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::PUT_METADATA;
    }
    else if (commands["list"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::LIST;
    }
    else if (commands["list_tiers"]->parsed()) {
        m_command              = Command::HSM;
        m_hsm_command.m_method = HsmCommand::Method::LIST_TIERS;
    }
    else if (commands["start"]->parsed()) {
        m_command = Command::DAEMON_START;
    }
    else if (commands["stop"]->parsed()) {
        m_command = Command::DAEMON_STOP;
    }
    else if (commands["server"]->parsed()) {
        m_command = Command::SERVER;
    }

    if (m_command == Command::UNKNOWN) {
        std::cerr << "Hestia: Empty CLI arguments. Use --help for usage: \n"
                  << app.help() << std::endl;
        throw std::runtime_error(
            "Returning after command line parsing. --help or invalid argument(s)");
    }

    if (!m_config_path.empty()) {
        m_config.load(m_config_path);
    }
    else {
        m_config.load_defaults();
    }
}

OpStatus HestiaCli::run()
{
    hestia::HestiaConfigurator configurator;
    const auto result = configurator.initialize(m_config);
    if (!result.ok()) {
        LOG_ERROR("Error configuring Hestia: " + result.message());
        return result;
    }

    if (m_command == Command::HSM) {
        return run_hsm();
    }
    else if (m_command == Command::SERVER) {
        return run_server();
    }
    else if (m_command == Command::DAEMON_START) {
        return start_daemon();
    }
    else if (m_command == Command::DAEMON_STOP) {
        return stop_daemon();
    }
    else {
        LOG_ERROR("CLI command not recognized");
        return {
            OpStatus::Status::ERROR, -1,
            "CLI command not supported. Use --help for options."};
    }
}

OpStatus HestiaCli::run_hsm()
{
    hestia::HsmServiceRequest::Ptr request;
    hestia::Stream::Ptr stream;

    if (m_hsm_command.m_method == HsmCommand::Method::GET) {
        LOG_INFO(
            "Handling GET CLI option - object: " + m_hsm_command.m_object_id);
        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(m_hsm_command.m_object_id),
            hestia::HsmServiceRequestMethod::GET);
        request->set_target_tier(m_hsm_command.m_target_tier);

        stream    = hestia::Stream::create();
        auto sink = hestia::FileStreamSink::create(m_hsm_command.m_path);
        stream->set_sink(std::move(sink));
    }
    else if (m_hsm_command.m_method == HsmCommand::Method::PUT) {
        LOG_INFO(
            "Handling PUT CLI option - object: " + m_hsm_command.m_object_id);

        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(m_hsm_command.m_object_id),
            hestia::HsmServiceRequestMethod::PUT);
        request->set_source_tier(m_hsm_command.m_source_tier);

        stream      = hestia::Stream::create();
        auto source = hestia::FileStreamSource::create(m_hsm_command.m_path);
        stream->set_source(std::move(source));
    }
    else if (m_hsm_command.m_method == HsmCommand::Method::COPY) {
        LOG_INFO(
            "Handling COPY CLI option - object: " + m_hsm_command.m_object_id);

        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(m_hsm_command.m_object_id),
            hestia::HsmServiceRequestMethod::COPY);
        request->set_source_tier(m_hsm_command.m_source_tier);
        request->set_target_tier(m_hsm_command.m_target_tier);
    }
    else if (m_hsm_command.m_method == HsmCommand::Method::MOVE) {
        LOG_INFO(
            "Handling MOVE CLI option - object: " + m_hsm_command.m_object_id);

        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(m_hsm_command.m_object_id),
            hestia::HsmServiceRequestMethod::MOVE);
        request->set_source_tier(m_hsm_command.m_source_tier);
        request->set_target_tier(m_hsm_command.m_target_tier);
    }
    else if (m_hsm_command.m_method == HsmCommand::Method::RELEASE) {
        LOG_INFO(
            "Handling RELEASE CLI option - object: "
            + m_hsm_command.m_object_id);

        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(m_hsm_command.m_object_id),
            hestia::HsmServiceRequestMethod::REMOVE);
        request->set_target_tier(m_hsm_command.m_target_tier);
    }
    else if (m_hsm_command.m_method == HsmCommand::Method::GET_METADATA) {
        LOG_INFO(
            "Handling GET_METADATA CLI option - object: "
            + m_hsm_command.m_object_id);

        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(m_hsm_command.m_object_id),
            hestia::HsmServiceRequestMethod::GET);
    }
    else if (m_hsm_command.m_method == HsmCommand::Method::PUT_METADATA) {
        LOG_INFO(
            "Handling PUT_METADATA CLI option - object: "
            + m_hsm_command.m_object_id);

        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(m_hsm_command.m_object_id),
            hestia::HsmServiceRequestMethod::PUT);
    }
    else if (m_hsm_command.m_method == HsmCommand::Method::LIST) {
        LOG_INFO(
            "Handling LIST CLI option - object: " + m_hsm_command.m_object_id);

        request = std::make_unique<hestia::HsmServiceRequest>(
            hestia::StorageObject(m_hsm_command.m_object_id),
            hestia::HsmServiceRequestMethod::LIST);
    }

    if (request) {
        auto response =
            hestia::ApplicationContext::get().get_hsm_service()->make_request(
                *(request.get()), stream.get());

        if (!response->ok()) {
            const std::string msg =
                "Request Failed with: " + response->get_error().to_string();
            LOG_ERROR(msg);
            return {OpStatus::Status::ERROR, -1, msg};
        }
    }

    if (stream) {
        auto stream_state = stream->flush();
        if (!stream_state.ok()) {
            const std::string msg =
                "Stream Failed with: " + stream_state.to_string();
            LOG_ERROR(msg);
            return {OpStatus::Status::ERROR, -1, msg};
        }
    }
    return {};
}

OpStatus HestiaCli::run_server()
{
    std::unique_ptr<HestiaService> hestia_service;
    std::unique_ptr<HsmS3Service> hsm_s3_service;

    WebApp::Ptr web_app;
    if (m_config.m_server_config.m_web_app == "hestia::HsmService") {
        HestiaServiceConfig service_config;
        auto hsm_service = ApplicationContext::get().get_hsm_service();
        auto kv_store    = std::make_unique<FileKeyValueStoreClient>();

        CurlClientConfig http_client_config;
        auto http_client = std::make_unique<CurlClient>(http_client_config);

        hestia_service = std::make_unique<HestiaService>(
            service_config, hsm_service, std::move(kv_store),
            std::move(http_client));

        web_app = std::make_unique<HestiaWebApp>(hestia_service.get());
    }
    else if (m_config.m_server_config.m_web_app == "hestia::HsmS3Service") {
        hsm_s3_service = std::make_unique<HsmS3Service>(
            ApplicationContext::get().get_hsm_service());
        web_app = std::make_unique<HestiaS3WebApp>(hsm_s3_service.get());
    }
    else if (m_config.m_server_config.m_web_app == "hestia::CopyTool") {
        web_app = std::make_unique<CopyToolWebApp>();
    }

    Server::Config server_config;
    server_config.m_ip        = m_config.m_server_config.m_host;
    server_config.m_http_port = std::stoi(m_config.m_server_config.m_port);
    server_config.m_block_on_launch = true;

    BasicHttpServer server(server_config, web_app.get());
    server.initialize();
    server.start();

    return {};
}

OpStatus HestiaCli::start_daemon()
{
    return {};
}

OpStatus HestiaCli::stop_daemon()
{
    return {};
}

}  // namespace hestia