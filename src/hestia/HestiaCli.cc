#include "HestiaCli.h"

#include "HestiaClient.h"

#include "FileStreamSink.h"
#include "FileStreamSource.h"

#include "ConsoleInterface.h"
#include "DaemonManager.h"
#include "Logger.h"
#include "UuidUtils.h"

#include <CLI/CLI11.hpp>
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

    for (const auto& action : m_client_command.get_subject_actions(subject)) {
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
                add_move_data_options(commands[tag]);
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
        ->add_option("file", m_client_command.m_path, "Path to write data to")
        ->required();
    command->add_option(
        "tier", m_client_command.m_source_tier, "Tier to get data from");
}

void HestiaCli::add_put_data_options(CLI::App* command)
{
    command
        ->add_option("file", m_client_command.m_path, "Path to read data from")
        ->required();
    command->add_option(
        "tier", m_client_command.m_target_tier, "Tier to put data to");
}

void HestiaCli::add_copy_data_options(CLI::App* command)
{
    command->add_option("source", m_client_command.m_source_tier, "Source Tier")
        ->required();
    command->add_option("target", m_client_command.m_target_tier, "Target Tier")
        ->required();
}

void HestiaCli::add_move_data_options(CLI::App* command)
{
    command->add_option("source", m_client_command.m_source_tier, "Source Tier")
        ->required();
    command->add_option("target", m_client_command.m_target_tier, "Target Tier")
        ->required();
}

void HestiaCli::add_release_data_options(CLI::App* command)
{
    command
        ->add_option(
            "tier", m_client_command.m_source_tier, "Tier to remove from")
        ->required();
}

void HestiaCli::add_crud_commands(
    std::unordered_map<std::string, CLI::App*>& commands,
    CLI::App& app,
    const std::string& subject)
{
    commands[subject] = app.add_subcommand(subject, subject + " commands");

    auto create_cmd =
        commands[subject]->add_subcommand("create", "Create a " + subject);
    create_cmd->add_option(
        "id_fmt", m_client_command.m_id_format, "Id Format Specifier");
    create_cmd->add_option(
        "input_fmt", m_client_command.m_input_format, "Input Format Specifier");
    create_cmd->add_option(
        "output_fmt", m_client_command.m_output_format,
        "Output Format Specifier");
    commands[subject + "_create"] = create_cmd;

    auto update_cmd =
        commands[subject]->add_subcommand("update", "Update a " + subject);
    update_cmd->add_option(
        "id_fmt", m_client_command.m_id_format, "Id Format Specifier");
    update_cmd->add_option(
        "attribute_fmt", m_client_command.m_input_format,
        "Attribute Format Specifier");
    update_cmd->add_option(
        "output_fmt", m_client_command.m_output_format,
        "Output Format Specifier");
    commands[subject + "_update"] = update_cmd;

    auto read_cmd =
        commands[subject]->add_subcommand("read", "Read " + subject + "s");
    read_cmd->add_option(
        "query_fmt", m_client_command.m_query_format, "Query Format Specifier");
    read_cmd->add_option(
        "output_fmt", m_client_command.m_output_format,
        "Output Format Specifier");
    read_cmd->add_option(
        "offset", m_client_command.m_offset, "Page start offset");
    read_cmd->add_option(
        "count", m_client_command.m_count, "Max number of items per page");
    commands[subject + "_read"] = read_cmd;

    auto remove_cmd =
        commands[subject]->add_subcommand("remove", "Remove a " + subject);
    remove_cmd->add_option(
        "id_fmt", m_client_command.m_id_format, "Id Format Specifier");
    commands[subject + "_remove"] = remove_cmd;

    auto identify_cmd =
        commands[subject]->add_subcommand("identify", "Identify a " + subject);
    identify_cmd->add_option(
        "id_fmt", m_client_command.m_id_format, "Id Format Spec");
    identify_cmd->add_option(
        "output_fmt", m_client_command.m_output_format,
        "Output Format Specifier");
    commands[subject + "_identify"] = identify_cmd;
}

void HestiaCli::parse_args(int argc, char* argv[])
{
    CLI::App app{
        "Hestia - Hierarchical Storage Tiers Interface for Applications",
        "hestia"};

    std::unordered_map<std::string, CLI::App*> commands;

    for (const auto& subject : m_client_command.get_hsm_subjects()) {
        add_crud_commands(commands, app, subject);
    }

    for (const auto& subject : m_client_command.get_system_subjects()) {
        add_crud_commands(commands, app, subject);
    }

    for (const auto& subject : m_client_command.get_action_subjects()) {
        add_hsm_actions(commands, commands[subject], subject);
    }

    commands["server"] = app.add_subcommand("server", "Run the Hestia Server");
    commands["server"]->add_option(
        "--host", m_server_host, "Hestia server host address");
    commands["server"]->add_option(
        "-p, --port", m_server_port, "Hestia server host port");

    commands["start"] = app.add_subcommand("start", "Start the Hestia Daemon");
    commands["stop"]  = app.add_subcommand("stop", "Stop the Hestia Daemon");

    for (auto& kv_pair : commands) {
        kv_pair.second->add_option(
            "-c, --config", m_config_path, "Path to a Hestia config file.");
        kv_pair.second->add_option(
            "-t, --token", m_config_path, "User authentication token.");
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

    for (const auto& subject : m_client_command.get_hsm_subjects()) {
        for (const auto& method : m_client_command.get_crud_methods()) {
            auto command = commands[subject + "_" + method];
            if (command != nullptr && command->parsed()) {
                m_client_command.set_crud_method(method);
                m_client_command.set_hsm_subject(subject);
                parsed = true;
                break;
            }
        }
        for (const auto& action :
             m_client_command.get_subject_actions(subject)) {
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
        for (const auto& method : m_client_command.get_crud_methods()) {
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

    if (commands["start"]->parsed()) {
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

OpStatus HestiaCli::run(IHestiaApplication* app)
{
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
        return run_client(app);
    }
    else {
        return {
            OpStatus::Status::ERROR, -1,
            "CLI command not supported. Use --help for options."};
    }
}

OpStatus HestiaCli::run_client(IHestiaApplication* app)
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
            "Unknown exception initializing client.");
    }

    if (!status.ok()) {
        return status;
    }

    auto client = dynamic_cast<IHestiaClient*>(app);
    if (client == nullptr) {
        return {
            OpStatus::Status::ERROR, -1,
            "Invalid app type passed to run_client."};
    }

    if (m_client_command.is_crud_method()) {
        return on_crud_method(client);
    }
    else if (m_client_command.is_data_management_action()) {
        const auto status =
            client->do_data_movement_action(m_client_command.m_action);
        return status;
    }
    else if (m_client_command.is_data_io_action()) {
        Stream stream;
        OpStatus status;
        if (m_client_command.is_data_put_action()) {
            stream.set_source(
                FileStreamSource::create(m_client_command.m_path));
            ;
        }
        else {
            stream.set_sink(FileStreamSink::create(m_client_command.m_path));
        }

        auto completion_cb =
            [&status](OpStatus ret_status, const HsmAction& action) {
                status = ret_status;
                (void)action;
            };

        client->do_data_io_action(
            m_client_command.m_action, &stream, completion_cb);
        (void)stream.flush();
        return status;
    }
    return {
        OpStatus::Status::ERROR, -1, "Unsupported client method requested."};
}

OpStatus HestiaCli::on_crud_method(IHestiaClient* client)
{
    if (m_client_command.is_create_method()) {
        VecCrudIdentifier ids;
        CrudAttributes attributes;
        const auto& [output_format, output_attr_format] =
            m_client_command.parse_create_update_inputs(
                ids, attributes, m_console_interface.get());

        if (const auto status = client->create(
                m_client_command.m_subject, ids, attributes,
                output_attr_format);
            !status.ok()) {
            return status;
        }

        if (HestiaClientCommand::expects_id(output_format)) {
            for (const auto& id : ids) {
                m_console_interface->console_write(id.get_primary_key() + "/n");
            }
            if (!attributes.buffer().empty()) {
                m_console_interface->console_write("/n");
            }
        }
        if (!attributes.buffer().empty()) {
            m_console_interface->console_write(attributes.buffer());
        }
    }
    else if (m_client_command.is_update_method()) {
        VecCrudIdentifier ids;
        CrudAttributes attributes;
        CrudAttributes::Format output_format{CrudAttributes::Format::JSON};
        if (const auto status = client->update(
                m_client_command.m_subject, ids, attributes, output_format);
            !status.ok()) {
            return status;
        }
        m_console_interface->console_write(attributes.buffer());
    }
    else if (m_client_command.is_read_method()) {

        CrudQuery query(CrudQuery::OutputFormat::ATTRIBUTES);
        if (const auto status = client->read(m_client_command.m_subject, query);
            status.ok()) {
            return status;
        }
        m_console_interface->console_write(query.m_attributes.buffer());
    }
    else if (m_client_command.is_remove_method()) {
        VecCrudIdentifier ids;
        return client->remove(m_client_command.m_subject, ids);
    }
    return {};
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