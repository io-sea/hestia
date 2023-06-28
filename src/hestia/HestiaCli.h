#pragma once

#include "Dictionary.h"
#include "ErrorUtils.h"
#include "HestiaConfig.h"

#include <filesystem>
#include <string>

namespace CLI {  // NOLINT
class App;
}

namespace hestia {

class HestiaCli {
  public:
    enum class Command { UNKNOWN, HSM, DAEMON_START, DAEMON_STOP, SERVER };

    struct HsmCommand {
        enum class Method {
            CREATE,
            PUT,
            GET,
            COPY,
            MOVE,
            RELEASE,
            LIST,
            LIST_TIERS,
            PUT_METADATA,
            GET_METADATA,
            UNKNOWN
        };
        Method m_method{Method::UNKNOWN};
        std::string m_object_id;
        std::string m_attributes;
        uint8_t m_source_tier{0};
        uint8_t m_target_tier{0};
        std::filesystem::path m_path;
    };

    void parse(int argc, char* argv[]);

    OpStatus run();

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

    void add_get_metadata_options(CLI::App* command);
    void add_put_metadata_options(CLI::App* command);

    void add_list_options(CLI::App* command);
    void add_list_tiers_options(CLI::App* command);

    OpStatus run_hsm();
    OpStatus run_server(const ServerConfig& config);
    OpStatus start_daemon();
    OpStatus stop_daemon();
};
}  // namespace hestia