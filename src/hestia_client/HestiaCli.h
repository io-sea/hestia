#pragma once

#include "Dictionary.h"
#include "HestiaConfig.h"

#include <string>

namespace CLI {  // NOLINT
class App;
}

namespace hestia {
class HestiaCli {
  public:
    enum class Command { UNKNOWN, HSM, DAEMON_START, DAEMON_STOP, SERVER };

    struct HsmCommand {
        enum class Method { PUT, GET, COPY, MOVE, RELEASE, UNKNOWN };
        Method m_method{Method::UNKNOWN};
        std::string m_object_id;
        uint8_t m_source_tier;
        uint8_t m_target_tier;
        std::string m_path;
    };

    void parse(int argc, char* argv[]);

    int run();

    Command m_command{Command::UNKNOWN};

    HsmCommand m_hsm_command;
    std::string m_config_path;
    Dictionary m_raw_config;
    HestiaConfig m_config;

  private:
    void add_get_options(CLI::App* command);
    void add_put_options(CLI::App* command);
    void add_copy_options(CLI::App* command);
    void add_move_options(CLI::App* command);
    void add_release_options(CLI::App* command);

    int run_hsm();
};
}  // namespace hestia