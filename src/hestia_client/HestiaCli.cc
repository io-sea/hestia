#include "HestiaCli.h"

#include "ApplicationContext.h"
#include "FileStreamSink.h"
#include "FileStreamSource.h"
#include "HestiaConfigurator.h"
#include "HsmService.h"

#include "Logger.h"

#include <iostream>
#include <sstream>
#include <unordered_map>

#include <CLI/CLI11.hpp>

namespace hestia {

void HestiaCli::add_get_options(CLI::App* command)
{
    command
        ->add_option("-o, --object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command
        ->add_option("-s, --source", m_hsm_command.m_source_tier, "Source Tier")
        ->required();
    command
        ->add_option(
            "-f, --file", m_hsm_command.m_path, "Path to write object data to")
        ->required();
}

void HestiaCli::add_put_options(CLI::App* command)
{
    command
        ->add_option("-o, --object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command
        ->add_option("-t, --target", m_hsm_command.m_target_tier, "Target Tier")
        ->required();
    command
        ->add_option("-f, --file", m_hsm_command.m_path, "Path to object data")
        ->required();
}

void HestiaCli::add_copy_options(CLI::App* command)
{
    command
        ->add_option("-o, --object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command
        ->add_option("-s, --source", m_hsm_command.m_source_tier, "Source Tier")
        ->required();
    command
        ->add_option("-t, --target", m_hsm_command.m_target_tier, "Target Tier")
        ->required();
}

void HestiaCli::add_move_options(CLI::App* command)
{
    command
        ->add_option("-o, --object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command
        ->add_option("-s, --source", m_hsm_command.m_source_tier, "Source Tier")
        ->required();
    command
        ->add_option("-t, --target", m_hsm_command.m_target_tier, "Target Tier")
        ->required();
}

void HestiaCli::add_release_options(CLI::App* command)
{
    command
        ->add_option("-o, --object_id", m_hsm_command.m_object_id, "Object Id")
        ->required();
    command
        ->add_option("-s, --source", m_hsm_command.m_source_tier, "Source Tier")
        ->required();
}

void HestiaCli::parse(int argc, char* argv[])
{
    CLI::App app{
        "Hestia - Hierarchical Storage Tiers Interface for Applications",
        "hestia"};

    std::unordered_map<std::string, CLI::App*> commands;
    commands["get"] = app.add_subcommand("get", "Get an object from the store");
    commands["put"] = app.add_subcommand("put", "Put an object in the store");
    commands["copy"] =
        app.add_subcommand("copy", "Copy an object between tiers");
    commands["move"] =
        app.add_subcommand("move", "Move an object between tiers");
    commands["release"] =
        app.add_subcommand("release", "Release an object from a tier");

    commands["start"]  = app.add_subcommand("start", "Start the Hestia Daemon");
    commands["stop"]   = app.add_subcommand("stop", "Stop the Hestia Daemon");
    commands["server"] = app.add_subcommand("server", "Run the Hestia Server");

    add_get_options(commands["get"]);
    add_put_options(commands["put"]);
    add_copy_options(commands["copy"]);
    add_move_options(commands["move"]);
    add_move_options(commands["release"]);

    commands["server"]
        ->add_option(
            "-h, --host", m_config.m_host, "Hestia server host address")
        ->required();
    commands["server"]
        ->add_option("-p, --port", m_config.m_port, "Hestia server host port")
        ->required();

    for (auto& kv_pair : commands) {
        kv_pair.second->set_config(
            "--config", "hestia.yaml", "Path to Hestia config file");
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
    else if (commands["start"]->parsed()) {
        m_command = Command::DAEMON_START;
    }
    else if (commands["stop"]->parsed()) {
        m_command = Command::DAEMON_STOP;
    }
    else if (commands["server"]->parsed()) {
        m_command = Command::SERVER;
    }
}

int HestiaCli::run()
{
    hestia::HestiaConfigurator configurator;
    auto result = configurator.initialize(m_config);
    if (result != 0) {
        std::cerr << "Error configuring Hestia" << std::endl;
        LOG_ERROR("Error configuring Hestia");
        return -1;
    }

    if (m_command == Command::HSM) {
        return run_hsm();
    }
    else {
        LOG_ERROR("CLI command not recognized");
        return -1;
    }
}

int HestiaCli::run_hsm()
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
    return 0;
}

}  // namespace hestia