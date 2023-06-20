#include "HestiaServer.h"

#include "ApplicationContext.h"
#include "DistributedHsmService.h"
#include "HsmS3Service.h"

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
HestiaServer::HestiaServer(const ServerConfig& config) : m_config(config) {}

void HestiaServer::run()
{
    WebApp::Ptr web_app;
    std::unique_ptr<HsmS3Service> hsm_s3_service;

    if (m_config.m_web_app_config.m_interface == WebAppConfig::Interface::S3) {
        LOG_INFO("Running S3 interface");
        hsm_s3_service = std::make_unique<HsmS3Service>(
            ApplicationContext::get().get_hsm_service(),
            ApplicationContext::get().get_kv_store_client());

        HestiaS3WebAppConfig config;
        web_app = std::make_unique<HestiaS3WebApp>(
            config, hsm_s3_service.get(),
            ApplicationContext::get().get_user_service());
    }
    else {
        LOG_INFO("Running http interface");

        HestiaWebAppConfig app_config;
        app_config.m_cache_static_resources = m_config.m_cache_static_resources;

        if (!m_config.m_static_resource_path.empty()) {
            auto full_path = std::filesystem::current_path()
                             / m_config.m_static_resource_path;
            if (std::filesystem::is_directory(full_path)) {
                app_config.m_static_resource_dir = full_path.string();
            }
        }
        web_app = std::make_unique<HestiaWebApp>(
            ApplicationContext::get().get_user_service(),
            ApplicationContext::get().get_hsm_service(), app_config);
    }

    Server::Config server_config;
    server_config.m_ip              = m_config.m_host;
    server_config.m_http_port       = std::stoi(m_config.m_port);
    server_config.m_block_on_launch = true;

    BasicHttpServer server(server_config, web_app.get());
    server.initialize();
    server.start();
}
}  // namespace hestia
