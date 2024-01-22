#include "Logger.h"

#include "CurlClient.h"
#include "HttpClient.h"
#include "S3ObjectStoreClient.h"

#include <CLI/CLI11.hpp>

#include <filesystem>
#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
    std::string resource_key;
    std::string bucket_key;
    std::string resource_path;

    bool activate_logger{false};

    CLI::App app{"Hestia S3 Tool", "hestia_s3_tool"};

    auto create_bucket =
        app.add_subcommand("create_bucket", "Create an s3 bucket.");

    auto put_object =
        app.add_subcommand("put_object", "Put an object to s3.");

    auto get_object =
        app.add_subcommand("get_object", "Get an object from s3.");

    std::vector<CLI::App*> commands = {create_bucket, put_object, get_object};

    for (auto command : commands) {
        command
            ->add_option(
                "--key", resource_key, "Key for the object or bucket.")
            ->required();
        command->add_option(
            "--activate_logger", activate_logger,
            "Whether to force the hestia logger to active.");
    }
    put_object->add_option(
        "--bucket", bucket_key, "Key for the bucket to place in - will be created if needed.")->required();

    put_object->add_option(
        "--file", resource_path, "Path to file with object content.")->required();

    put_object->add_option(
        "--file", resource_path, "Path to file with object content.")->required();

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