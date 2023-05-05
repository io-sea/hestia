#include "HestiaCli.h"

#include <CLI/CLI11.hpp>

namespace hestia {

void HestiaCli::parse(int argc, char* argv[])
{
    CLI::App parser;

    std::string method_string;
    parser.add_option(
        "-m, --method", method_string, "HSM Method - PUT/GET etc.");
    parser.add_option("-o, --object_id", m_object_id, "Object Id")->required();
    parser.add_option("-s, --source", m_source_tier, "Source Tier");
    parser.add_option("-t, --target", m_target_tier, "Target Tier");
    parser.add_option("-f, --file", m_path, "Object data file path");
    parser.set_config("--config");

    try {
        parser.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        parser.exit(e);
        throw std::runtime_error(
            "Returning after command line parsing. --help or invalid argument(s)");
    }

    if (method_string == "PUT") {
        m_method = Method::PUT;
    }
    else if (method_string == "GET") {
        m_method = Method::GET;
    }
    else if (method_string == "COPY") {
        m_method = Method::COPY;
    }
    else if (method_string == "MOVE") {
        m_method = Method::MOVE;
    }
    else if (method_string == "RELEASE") {
        m_method = Method::RELEASE;
    }
}
}  // namespace hestia