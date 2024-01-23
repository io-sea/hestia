#pragma once

#include "ConsoleInterface.h"
#include "HestiaApplication.h"
#include "HestiaCommands.h"

namespace CLI {  // NOLINT
class App;
}

namespace hestia {

class IHestiaClient;
class IConsoleInterface;

class HestiaCli {
  public:
    enum class AppCommand { UNKNOWN, CLIENT, SERVER };

    HestiaCli(std::unique_ptr<IConsoleInterface> console_interface = {});

    bool is_client() const;

    bool is_server() const;

    void parse_args(int argc, char* argv[]);

    OpStatus run(IHestiaApplication* app, bool skip_init = false);

  private:
    void add_crud_commands(
        std::unordered_map<std::string, CLI::App*>& commands,
        CLI::App& app,
        const std::string& subject);
    void on_crud_option(const std::string& subject, const std::string& method);

    void add_hsm_actions(
        std::unordered_map<std::string, CLI::App*>& commands,
        CLI::App* command,
        const std::string& subject);
    void add_get_data_options(CLI::App* command);
    void add_put_data_options(CLI::App* command);
    void add_copy_data_options(CLI::App* command);
    void add_move_data_options(CLI::App* command);
    void add_release_data_options(CLI::App* command);

    OpStatus run_client(IHestiaApplication* app, bool skip_init);

    OpStatus on_client_request(IHestiaClient& client, HestiaRequest& req);

    OpStatus run_server(IHestiaApplication* app);

    OpStatus print_info(IHestiaApplication* app);
    void print_version();

    void reset_cli();

    std::string m_user_token;
    std::string m_config_path;
    std::string m_server_host;
    unsigned m_server_port{8080};
    AppCommand m_app_command{AppCommand::UNKNOWN};
    HestiaClientCommand m_client_command;
    std::string m_console_prefix{"Hestia> "};
    std::unique_ptr<IConsoleInterface> m_console_interface;
};
}  // namespace hestia