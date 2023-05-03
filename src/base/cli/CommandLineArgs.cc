#include "CommandLineArgs.h"

namespace hestia {
void CommandLineArgs::add_option(const CommandLineOption& option)
{
    m_options.push_back(option);
}

void CommandLineArgs::parse_args(int argc, char** argv) {}

const std::vector<CommandLineArgs::Arg>& CommandLineArgs::get_args() const
{
    return m_args;
}
}  // namespace hestia