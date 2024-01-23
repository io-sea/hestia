#pragma once

#include "HestiaApplication.h"

#include "ServerConfig.h"

#include <future>

namespace hestia {
class Server;
class HestiaWebAppConfig;
class WebApp;

class HestiaServer : public HestiaApplication {
  public:
    HestiaServer();

    virtual ~HestiaServer();

    OpStatus run() override;

  private:
    void sync_web_app_config(HestiaWebAppConfig& config);

    void set_app_mode(const std::string& host, unsigned port) override;

    void add_server(WebApp* app, unsigned port = 0);

    std::vector<std::unique_ptr<WebApp>> m_apps;
    std::vector<std::unique_ptr<Server>> m_servers;
    std::vector<std::future<int>> m_server_tasks;
};
}  // namespace hestia