#include "HestiaCli.h"

#include <iostream>
#include <sstream>

#include <CLI/CLI11.hpp>

namespace hestia {

void HestiaCli::parse(int argc, char* argv[])
{
    CLI::App app{
        "Hestia - Hierarchical Storage Tiers Interface for Applications",
        "hestia"};

    auto get_command =
        app.add_subcommand("get", "Get an object from the store");
    auto put_command = app.add_subcommand("put", "Put an object in the store");
    auto copy_command =
        app.add_subcommand("copy", "Copy an object between tiers");
    auto move_command =
        app.add_subcommand("move", "Move an object between tiers");
    auto release_command =
        app.add_subcommand("release", "Release an object from a tier");

    get_command->add_option("-o, --object_id", m_object_id, "Object Id")
        ->required();
    get_command->add_option("-s, --source", m_source_tier, "Source Tier")
        ->required();
    get_command
        ->add_option("-f, --file", m_path, "Path to write object data to")
        ->required();

    put_command->add_option("-o, --object_id", m_object_id, "Object Id")
        ->required();
    put_command->add_option("-t, --target", m_target_tier, "Target Tier")
        ->required();
    put_command->add_option("-f, --file", m_path, "Path to object data")
        ->required();

    copy_command->add_option("-o, --object_id", m_object_id, "Object Id")
        ->required();
    copy_command->add_option("-s, --source", m_source_tier, "Source Tier")
        ->required();
    copy_command->add_option("-t, --target", m_target_tier, "Target Tier")
        ->required();

    move_command->add_option("-o, --object_id", m_object_id, "Object Id")
        ->required();
    move_command->add_option("-s, --source", m_source_tier, "Source Tier")
        ->required();
    move_command->add_option("-t, --target", m_target_tier, "Target Tier")
        ->required();

    release_command->add_option("-o, --object_id", m_object_id, "Object Id")
        ->required();
    release_command->add_option("-s, --source", m_source_tier, "Source Tier")
        ->required();

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        app.exit(e);
        throw std::runtime_error(
            "Returning after command line parsing. --help or invalid argument(s)");
    }

    if (put_command->parsed()) {
        m_method = Method::PUT;
    }
    else if (get_command->parsed()) {
        m_method = Method::GET;
    }
    else if (copy_command->parsed()) {
        m_method = Method::COPY;
    }
    else if (move_command->parsed()) {
        m_method = Method::MOVE;
    }
    else if (release_command->parsed()) {
        m_method = Method::RELEASE;
    }
}
}  // namespace hestia