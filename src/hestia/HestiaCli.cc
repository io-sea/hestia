#include "HestiaCli.h"

#include "HestiaClient.h"

#include "FileStreamSink.h"
#include "FileStreamSource.h"

#include "ConsoleInterface.h"
#include "DaemonManager.h"
#include "ErrorUtils.h"
#include "Logger.h"
#include "StringUtils.h"
#include "UuidUtils.h"

#include "ProjectConfig.h"

#include <CLI/CLI11.hpp>
#include <future>
#include <iostream>

namespace hestia {

HestiaCli::HestiaCli(std::unique_ptr<IConsoleInterface> console_interface) :
    m_console_interface(
        console_interface == nullptr ? std::make_unique<ConsoleInterface>() :
                                       std::move(console_interface))
{
}

void HestiaCli::add_hsm_actions(
    std::unordered_map<std::string, CLI::App*>& commands,
    CLI::App* command,
    const std::string& subject)
{
    HsmAction::Action_enum_string_converter converter;
    converter.init();

    for (const auto& action : HsmAction::get_subject_actions(subject)) {
        const auto action_name = converter.to_string(action);
        const auto tag         = subject + "_" + action_name;

        switch (action) {
            case HsmAction::Action::GET_DATA:
                commands[tag] = command->add_subcommand(
                    action_name, "Get " + subject + " data");
                add_get_data_options(commands[tag]);
                break;
            case HsmAction::Action::PUT_DATA:
                commands[tag] = command->add_subcommand(
                    action_name, "Put " + subject + " data");
                add_put_data_options(commands[tag]);
                break;
            case HsmAction::Action::COPY_DATA:
                commands[tag] = command->add_subcommand(
                    action_name, "Copy a " + subject + " between tiers");
                add_copy_data_options(commands[tag]);
                break;
            case HsmAction::Action::MOVE_DATA:
                commands[tag] = command->add_subcommand(
                    action_name, "Move a " + subject + " between tiers");
                add_move_data_options(commands[tag]);
                break;
            case HsmAction::Action::RELEASE_DATA:
                commands[tag] = command->add_subcommand(
                    action_name, "Release a " + subject + " from a tier");
                add_release_data_options(commands[tag]);
                break;
            case HsmAction::Action::CRUD:
            case HsmAction::Action::NONE:
            default:
                continue;
        };
        commands[tag]
            ->add_option("id", m_client_command.m_id, "Id")
            ->required();
    }
}

void HestiaCli::add_get_data_options(CLI::App* command)
{
    command
        ->add_option("--file", m_client_command.m_path, "Path to write data to")
        ->required();
    command->add_option(
        "--tier", m_client_command.m_source_tier, "Tier to get data from");
}

void HestiaCli::add_put_data_options(CLI::App* command)
{
    command
        ->add_option(
            "--file", m_client_command.m_path, "Path to read data from")
        ->required();
    command->add_option(
        "--tier", m_client_command.m_target_tier, "Tier to put data to");
}

void HestiaCli::add_copy_data_options(CLI::App* command)
{
    command
        ->add_option("--source", m_client_command.m_source_tier, "Source Tier")
        ->required();
    command
        ->add_option("--target", m_client_command.m_target_tier, "Target Tier")
        ->required();
}

void HestiaCli::add_move_data_options(CLI::App* command)
{
    command
        ->add_option("--source", m_client_command.m_source_tier, "Source Tier")
        ->required();
    command
        ->add_option("--target", m_client_command.m_target_tier, "Target Tier")
        ->required();
}

void HestiaCli::add_release_data_options(CLI::App* command)
{
    command
        ->add_option(
            "--tier", m_client_command.m_source_tier, "Tier to remove from")
        ->required();
}

void HestiaCli::add_crud_commands(
    std::unordered_map<std::string, CLI::App*>& commands,
    CLI::App& app,
    const std::string& subject)
{
    commands[subject] = app.add_subcommand(subject, subject + " commands");

    const std::string id_format_help =
        "Id Format Specifier. One of 'id', 'name', 'parent_id', 'parent_name'";
    const std::string input_format_help =
        "Input Format Specifier. One of 'none', 'id', 'json', 'key_value'.";
    const std::string output_format_help =
        "Output Format Specifier. One of 'none', 'id', 'json', 'key_value'.";

    auto create_cmd =
        commands[subject]->add_subcommand("create", "Create a " + subject);
    create_cmd->add_option(
        "--id_fmt", m_client_command.m_id_format, id_format_help);
    create_cmd->add_option(
        "--input_fmt", m_client_command.m_input_format, input_format_help);
    create_cmd->add_option(
        "--output_fmt", m_client_command.m_output_format, output_format_help);
    create_cmd->add_option("id", m_client_command.m_id, "Subject Id");
    commands[subject + "_create"] = create_cmd;

    auto update_cmd =
        commands[subject]->add_subcommand("update", "Update a " + subject);
    update_cmd->add_option(
        "--id_fmt", m_client_command.m_id_format, id_format_help);
    update_cmd->add_option(
        "--input_fmt", m_client_command.m_input_format, input_format_help);
    update_cmd->add_option(
        "--output_fmt", m_client_command.m_output_format, output_format_help);
    update_cmd->add_option("id", m_client_command.m_id, "Subject Id");
    commands[subject + "_update"] = update_cmd;

    auto read_cmd =
        commands[subject]->add_subcommand("read", "Read " + subject + "s");
    read_cmd->add_option(
        "--query_fmt", m_client_command.m_input_format,
        "Query Format Specifier");
    read_cmd->add_option(
        "--id_fmt", m_client_command.m_id_format, id_format_help);
    read_cmd->add_option(
        "--output_fmt", m_client_command.m_output_format, output_format_help);
    read_cmd->add_option(
        "--offset", m_client_command.m_offset, "Page start offset");
    read_cmd->add_option(
        "--count", m_client_command.m_count, "Max number of items per page");
    read_cmd->add_option("query", m_client_command.m_body, "Query body");
    commands[subject + "_read"] = read_cmd;

    auto remove_cmd =
        commands[subject]->add_subcommand("remove", "Remove a " + subject);
    remove_cmd->add_option(
        "--id_fmt", m_client_command.m_id_format, id_format_help);
    remove_cmd->add_option("id", m_client_command.m_id, "Subject Id");
    commands[subject + "_remove"] = remove_cmd;

    auto identify_cmd =
        commands[subject]->add_subcommand("identify", "Identify a " + subject);
    identify_cmd->add_option(
        "--id_fmt", m_client_command.m_id_format, id_format_help);
    identify_cmd->add_option(
        "--output_fmt", m_client_command.m_output_format, output_format_help);
    commands[subject + "_identify"] = identify_cmd;
}

void HestiaCli::reset_cli()
{
    m_client_command = HestiaClientCommand();
    m_user_token.clear();
    m_config_path.clear();
    m_server_host.clear();
}

void HestiaCli::parse_args(int argc, char* argv[])
{
    reset_cli();

    CLI::App app{
        "Hestia - Hierarchical Storage Tiers Interface for Applications",
        "hestia"};

    std::unordered_map<std::string, CLI::App*> commands;

    for (const auto& subject : HsmItem::get_hsm_subjects()) {
        add_crud_commands(commands, app, subject);
    }

    for (const auto& subject : m_client_command.get_system_subjects()) {
        add_crud_commands(commands, app, subject);
    }

    for (const auto& subject : HsmAction::get_action_subjects()) {
        add_hsm_actions(commands, commands[subject], subject);
    }

    commands["server"] = app.add_subcommand("server", "Run the Hestia Server");

    commands["start"] = app.add_subcommand("start", "Start the Hestia Daemon");
    commands["stop"]  = app.add_subcommand("stop", "Stop the Hestia Daemon");

    app.add_flag(
        "--version", m_client_command.m_is_version,
        "Print application version");

    for (const auto& [key, command] : commands) {
        if (!command->get_subcommands().empty()) {
            continue;
        }

        command->add_option(
            "--verbosity", m_client_command.m_verbosity,
            "Controls verbosity level - default is 2. Value 0 stops all output.");
        command->add_option(
            "-c, --config", m_config_path, "Path to a Hestia config file.");
        command->add_option(
            "-t, --token", m_user_token, "User authentication token.");
        command->add_option(
            "--host", m_server_host, "Hestia server host address");
        command->add_option(
            "-p, --port", m_server_port, "Hestia server host port");
    }

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        app.exit(e);
        throw std::runtime_error(
            "Returning after command line parsing. --help or invalid argument(s)");
    }

    bool parsed{false};
    HsmAction::Action_enum_string_converter action_string_converter;
    action_string_converter.init();

    for (const auto& subject : HsmItem::get_hsm_subjects()) {
        for (const auto& method : BaseCrudRequest::get_crud_methods()) {
            auto command = commands[subject + "_" + method];
            if (command != nullptr && command->parsed()) {
                m_client_command.set_crud_method(method);
                m_client_command.set_hsm_subject(subject);
                parsed = true;
                break;
            }
        }
        for (const auto& action : HsmAction::get_subject_actions(subject)) {
            const auto action_str = action_string_converter.to_string(action);
            if (commands[subject + "_" + action_str]->parsed()) {
                m_client_command.set_hsm_action(action);
                m_client_command.set_hsm_subject(subject);
                parsed = true;
                break;
            }
        }
    }

    for (const auto& subject : m_client_command.get_system_subjects()) {
        for (const auto& method : BaseCrudRequest::get_crud_methods()) {
            auto command = commands[subject + "_" + method];
            if (command != nullptr && command->parsed()) {
                m_client_command.set_crud_method(method);
                m_client_command.set_system_subject(subject);
                parsed = true;
                break;
            }
        }
    }

    if (parsed) {
        m_app_command = AppCommand::CLIENT;
        return;
    }

    if (m_client_command.m_is_version) {
        print_version();
        return;
    }
    else if (commands["start"]->parsed()) {
        m_app_command = AppCommand::DAEMON_START;
    }
    else if (commands["stop"]->parsed()) {
        m_app_command = AppCommand::DAEMON_STOP;
    }
    else if (commands["server"]->parsed()) {
        m_app_command = AppCommand::SERVER;
    }

    if (m_app_command == AppCommand::UNKNOWN) {
        std::cerr << "Hestia: Empty CLI arguments. Use --help for usage: \n"
                  << app.help() << std::endl;
        throw std::runtime_error(
            "Returning after command line parsing. --help or invalid argument(s)");
    }
}

bool HestiaCli::is_client() const
{
    return m_app_command == AppCommand::CLIENT;
}

bool HestiaCli::is_server() const
{
    return m_app_command == AppCommand::DAEMON_START
           || m_app_command == AppCommand::SERVER;
}

bool HestiaCli::is_daemon() const
{
    return m_app_command == AppCommand::DAEMON_START
           || m_app_command == AppCommand::DAEMON_STOP;
}

OpStatus HestiaCli::run(IHestiaApplication* app, bool skip_init)
{
    if (m_client_command.m_is_version) {
        return {};
    }

    if (app == nullptr || m_app_command == AppCommand::DAEMON_STOP) {
        return stop_daemon();
    }
    else if (m_app_command == AppCommand::DAEMON_START) {
        return start_daemon(app);
    }
    else if (m_app_command == AppCommand::SERVER) {
        return run_server(app);
    }
    else if (m_app_command == AppCommand::CLIENT) {
        return run_client(app, skip_init);
    }
    else {
        return {
            OpStatus::Status::ERROR, -1,
            "CLI command not supported. Use --help for options."};
    }
}

OpStatus HestiaCli::print_info(IHestiaApplication* app)
{
    m_console_interface->console_write_error(app->get_runtime_info());
    return {};
}

void HestiaCli::print_version()
{
    m_console_interface->console_write(
        hestia::project_config::get_project_name()
        + " version: " + hestia::project_config::get_project_version());
}

OpStatus HestiaCli::run_client(IHestiaApplication* app, bool skip_init)
{
    if (!skip_init) {
        OpStatus status;
        try {
            status = app->initialize(
                m_config_path, m_user_token, {}, m_server_host, m_server_port);
        }
        catch (const std::exception& e) {
            status = OpStatus(OpStatus::Status::ERROR, -1, e.what());
        }
        catch (...) {
            status = OpStatus(
                OpStatus::Status::ERROR, -1,
                "Unknown exception initializing client.");
        }

        if (!status.ok()) {
            return status;
        }
    }

    if (m_client_command.m_verbosity == 3) {
        print_info(app);
    }

    if (m_client_command.m_verbosity == 2) {
        m_console_interface->console_write(
            m_console_prefix + "Started Hestia Client.");
    }

    auto client = dynamic_cast<IHestiaClient*>(app);
    if (client == nullptr) {
        return {
            OpStatus::Status::ERROR, -1,
            "Invalid app type passed to run_client."};
    }

    if (m_client_command.is_crud_method()) {
        HestiaRequest req(
            m_client_command.m_subject, m_client_command.get_crud_method());
        m_client_command.parse_console_inputs(*m_console_interface, req);
        const auto status = on_client_request(*client, req);
        if (m_client_command.m_verbosity == 2) {
            m_console_interface->console_write(
                m_console_prefix + "Finished Hestia Client.");
        }
        return status;
    }
    else {
        HestiaRequest req(
            m_client_command.m_subject, m_client_command.m_action);
        m_client_command.parse_action(req);
        const auto status = on_client_request(*client, req);
        if (m_client_command.m_verbosity == 2) {
            m_console_interface->console_write(
                m_console_prefix + "Finished Hestia Client.");
        }
        return status;
    }
}

OpStatus HestiaCli::on_client_request(IHestiaClient& client, HestiaRequest& req)
{
    Stream stream;
    if (req.supports_stream_source()) {
        stream.set_source(FileStreamSource::create(m_client_command.m_path));
        req.action().set_size(stream.get_source_size());
    }
    else if (req.supports_stream_sink()) {
        stream.set_sink(FileStreamSink::create(m_client_command.m_path));
    }

    std::promise<HestiaResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto progress_cb = [this](const HsmAction& action) {
        m_console_interface->console_write(
            "Processing HSM action: " + action.get_primary_key());
    };

    auto completion_cb = [&response_promise](HestiaResponse::Ptr response) {
        response_promise.set_value(std::move(response));
    };

    if (m_client_command.m_verbosity == 2) {
        std::string msg = m_console_prefix;
        if (req.is_crud_request()) {

            if (req.is_hsm_type()) {
                msg += "Doing " + req.get_crud_request().method_as_string()
                       + " on "
                       + StringUtils::to_upper(req.get_hsm_type_as_string());
            }
            else {
                std::string system_type = req.is_user_type() ? "user" : "node";
                msg += "Doing " + req.get_crud_request().method_as_string()
                       + " on " + system_type;
            }

            if (req.get_crud_request().get_ids().size() == 1) {
                msg += " with id: "
                       + req.get_crud_request()
                             .get_ids()
                             .first()
                             .get_primary_key();
            }
            else if (!req.get_crud_request().get_ids().empty()) {
                msg += " with "
                       + std::to_string(req.get_crud_request().get_ids().size())
                       + " ids.";
            }
        }
        else {
            msg +=
                "Doing HSM Action "
                + StringUtils::to_upper(req.get_action().get_action_as_string())
                + " on id " + req.get_action().get_subject_key();
        }
        m_console_interface->console_write(msg);
    }

    client.make_request(req, completion_cb, &stream, progress_cb);

    if (req.supports_stream_source()) {
        if (stream.waiting_for_content()) {
            LOG_INFO("Stream waiting for content - starting flush");
            auto result = stream.flush();
            if (!result.ok()) {
                const auto msg =
                    "Failed to flush stream with: " + result.to_string();
                LOG_ERROR(msg);
                return {OpStatus::Status::ERROR, -1, msg};
            }
        }
    }
    else if (req.supports_stream_sink()) {
        LOG_INFO("Req type supports stream");
        if (stream.has_content()) {
            LOG_INFO("Stream has content - starting flush");
            auto result = stream.flush();
            if (!result.ok()) {
                const auto msg =
                    "Failed to flush stream with: " + result.to_string();
                LOG_ERROR(msg);
                return {OpStatus::Status::ERROR, -1, msg};
            }
        }
    }

    const auto response = response_future.get();
    if (response->ok()) {
        CrudQuery::FormatSpec format;
        format.m_attrs_format.m_json_format.m_collapse_single = true;
        format.m_attrs_format.m_json_format.set_indent(4);

        if (m_client_command.m_verbosity == 2) {
            std::string msg = m_console_prefix;
            if (req.is_crud_request()) {
                msg += "Completed " + req.get_crud_request().method_as_string()
                       + " on "
                       + StringUtils::to_upper(req.get_hsm_type_as_string())
                       + ". With result:";
            }
            else {
                msg += "Completed HSM Action: "
                       + StringUtils::to_upper(
                           req.get_action().get_action_as_string())
                       + " on id " + req.get_action().get_subject_key()
                       + " with action id: ";
            }
            m_console_interface->console_write(msg);
        }
        m_console_interface->console_write(response->write(format));
    }
    return response->get_status();
}

OpStatus HestiaCli::run_server(IHestiaApplication* app)
{
    OpStatus status;
    try {
        status = app->initialize(m_config_path, m_user_token);
    }
    catch (const std::exception& e) {
        status = OpStatus(OpStatus::Status::ERROR, -1, e.what());
    }
    catch (...) {
        status = OpStatus(
            OpStatus::Status::ERROR, -1,
            "Unknown exception initializing server.");
    }

    if (!status.ok()) {
        return status;
    }

    if (m_client_command.m_verbosity == 2) {
        print_version();
        print_info(app);
    }

    try {
        status = app->run();
    }
    catch (const std::exception& e) {
        status = OpStatus(OpStatus::Status::ERROR, -1, e.what());
    }
    catch (...) {
        status = OpStatus(
            OpStatus::Status::ERROR, -1, "Unknown exception running server.");
    }
    return status;
}

OpStatus HestiaCli::start_daemon(IHestiaApplication* app)
{
    DaemonManager daemon_manager;
    auto rc = daemon_manager.start();
    if (rc == DaemonManager::Status::EXIT_OK) {
        return {};
    }
    else if (rc == DaemonManager::Status::EXIT_FAILED) {
        return OpStatus(
            OpStatus::Status::ERROR, 0, "Error starting hestia Daemon");
    }
    return run_server(app);
}

OpStatus HestiaCli::stop_daemon()
{
    DaemonManager daemon_manager;
    const auto rc = daemon_manager.stop();
    if (rc == DaemonManager::Status::EXIT_OK) {
        return {};
    }
    else if (rc == DaemonManager::Status::EXIT_FAILED) {
        return OpStatus(
            OpStatus::Status::ERROR, 0, "Error stopping hestia Daemon");
    }
    return {};
}

}  // namespace hestia