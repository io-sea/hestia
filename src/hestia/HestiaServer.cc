#include "HestiaServer.h"

#include "DistributedHsmService.h"

#include "HsmObjectStoreClient.h"
#include "HttpClient.h"
#include "KeyValueStoreClient.h"

#include "BasicHttpServer.h"

#include "HestiaS3WebApp.h"
#include "HestiaWebApp.h"

#include "ProjectConfig.h"

#include <filesystem>
#include <set>

#include "Logger.h"

namespace hestia {
HestiaServer::HestiaServer() {}

HestiaServer::~HestiaServer() {}

OpStatus HestiaServer::run()
{
    for (const auto& app_config :
         m_config.get_server_config().get_interfaces()) {
        WebApp* web_app{nullptr};
        if (app_config.get_type() == HsmNodeInterface::Type::S3) {
            LOG_INFO("Adding S3 interface");
            HestiaS3WebAppConfig config;
            web_app = m_apps
                          .emplace_back(std::make_unique<HestiaS3WebApp>(
                              config, m_distributed_hsm_service.get(),
                              m_user_service.get()))
                          .get();
        }
        else {
            LOG_INFO("Adding http interface");
            HestiaWebAppConfig http_app_config;
            sync_web_app_config(http_app_config);
            web_app = m_apps
                          .emplace_back(std::make_unique<HestiaWebApp>(
                              m_user_service.get(),
                              m_distributed_hsm_service.get(), http_app_config))
                          .get();
        }
        add_server(web_app, app_config.get_port());
    }

    if (m_servers.empty()) {
        LOG_INFO("No apps specified - defaulting to http interface");
        HestiaWebAppConfig http_app_config;
        sync_web_app_config(http_app_config);
        auto web_app =
            m_apps
                .emplace_back(std::make_unique<HestiaWebApp>(
                    m_user_service.get(), m_distributed_hsm_service.get(),
                    http_app_config))
                .get();
        add_server(web_app);
    }

    if (m_servers.size() == 1) {
        m_servers[0]->start();
        if (!m_config.get_server_config().should_block_on_launch()) {
            m_servers[0]->wait_until_bound();
        }
        else {
            m_servers.clear();
        }
    }
    else {
        bool should_block =
            m_config.get_server_config().should_block_on_launch();
        for (auto& server : m_servers) {
            auto launch_func = [&server, should_block]() {
                server->start();
                if (!should_block) {
                    server->wait_until_bound();
                }
                else {
                    server.reset();
                }
                return 0;
            };
            m_server_tasks.emplace_back(
                std::async(std::launch::async, launch_func));
        }
        for (auto& task : m_server_tasks) {
            task.get();
        }
        m_servers.clear();
    }
    return {};
}

void HestiaServer::add_server(WebApp* app, unsigned port)
{
    Server::Config server_config;
    server_config.m_ip = m_config.get_server_config().get_host_address();
    if (port == 0) {
        server_config.m_http_port = m_config.get_server_config().get_port();
    }
    else {
        server_config.m_http_port = port;
    }
    server_config.m_block_on_launch =
        m_config.get_server_config().should_block_on_launch();

    auto server =
        m_servers
            .emplace_back(std::make_unique<BasicHttpServer>(server_config, app))
            .get();
    server->initialize();
}

void HestiaServer::sync_web_app_config(HestiaWebAppConfig& config)
{
    const auto static_resource_path =
        m_config.get_server_config().get_static_resource_path();

    config.m_cache_static_resources =
        m_config.get_server_config().should_cache_static_resources();

    if (!static_resource_path.empty()) {
        auto full_path = std::filesystem::current_path() / static_resource_path;
        if (std::filesystem::is_directory(full_path)) {
            config.m_static_resource_dir = full_path.string();
        }
    }
}

void HestiaServer::set_app_mode(const std::string&, unsigned)
{
    if (!m_config.get_server_config().has_controller_address()) {
        LOG_INFO("Running Server in Controller mode");
        m_app_mode = ApplicationMode::SERVER_CONTROLLER;
    }
    else {
        LOG_INFO("Running Server in Worker mode");
        m_app_mode = ApplicationMode::SERVER_WORKER;
    }
}
}  // namespace hestia
