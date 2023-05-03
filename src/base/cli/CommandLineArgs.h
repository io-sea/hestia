#pragma once

#include <string>
#include <vector>

namespace hestia {
struct CommandLineOption {
    std::string m_name;
    std::string m_short_flag;
    std::string m_default_value;
    std::string m_description;
};

class CommandLineArgs {
  public:
    using Arg = std::pair<std::string, std::string>;

    void add_option(const CommandLineOption& option);

    const std::vector<Arg>& get_args() const;

    void parse_args(int argc, char** argv);

  private:
    std::vector<CommandLineOption> m_options;
    std::vector<Arg> m_args;
};
}  // namespace hestia