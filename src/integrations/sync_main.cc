#include "SyncTool.h"

#include "Logger.h"

#include <CLI/CLI11.hpp>

#include <filesystem>
#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
    std::string command_name;
    std::string work_dir;
    std::string dataset_id;
    std::string config_path;
    std::size_t sample_frequency{10};
    std::string sample_path;
    bool activate_logger{false};

    CLI::App app{"Hestia Sync Tool", "hestia_sync_tool"};
    auto archive_command =
        app.add_subcommand("archive", "Archive the directory to Hestia");
    auto restore_command =
        app.add_subcommand("restore", "Restore the directory from Hestia");
    auto sync_command =
        app.add_subcommand("sync", "Sync the directory with Hestia");
    std::vector<CLI::App*> commands = {
        archive_command, restore_command, sync_command};
    for (auto command : commands) {
        command
            ->add_option(
                "--work_dir", work_dir, "Directory to archive or restore.")
            ->required();
        command->add_option(
            "--config", config_path, "Path to the hestia config.");
        command->add_option(
            "--activate_logger", activate_logger,
            "Whether to force the hestia logger to active.");
    }
    archive_command->add_option(
        "--sample_frequency", sample_frequency,
        "How often to take file op timings.");
    archive_command->add_option(
        "--sample_path", sample_path, "File to write samples to.");

    restore_command
        ->add_option(
            "--dataset_id", dataset_id, "Label for dataset to work with.")
        ->required();
    sync_command
        ->add_option(
            "--dataset_id", dataset_id, "Label for dataset to work with.")
        ->required();

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        app.exit(e);
        throw std::runtime_error(
            "Returning after command line parsing. --help or invalid argument(s)");
    }

    auto sync_dir = std::filesystem::path(work_dir);
    if (sync_dir.is_relative()) {
        sync_dir = std::filesystem::current_path() / sync_dir;
    }
    if (!std::filesystem::is_directory(sync_dir)) {
        std::cerr << "Expects existing directory for 'work_dir'" << std::endl;
    }

    if (activate_logger) {
        hestia::LoggerConfig logger_config;
        logger_config.set_active(true);
        logger_config.set_level(hestia::LoggerConfig::Level::DEBUG);
        logger_config.set_console_only(true);
        logger_config.set_should_assert_on_error(false);
        hestia::Logger::get_instance().do_initialize({}, logger_config);
    }

    hestia::SyncTool sync_tool(sync_dir, config_path);
    if (archive_command->parsed()) {
        sync_tool.archive(sample_path, sample_frequency);
    }
    else if (restore_command->parsed()) {
        sync_tool.restore(dataset_id);
    }
    else if (sync_command->parsed()) {
        sync_tool.sync(dataset_id);
    }
    return 0;
}