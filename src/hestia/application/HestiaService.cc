#include "HestiaService.h"

#include "ApplicationContext.h"
#include "DistributedHsmService.h"
#include "HsmS3Service.h"

#include "HttpClient.h"
#include "KeyValueStoreClient.h"

#include "BasicHttpServer.h"

#include "HestiaS3WebApp.h"
#include "HestiaWebApp.h"

#include "Logger.h"

namespace hestia {
HestiaService::HestiaService(const ServerConfig& config) : m_config(config) {}

void HestiaService::run()
{
    DistributedHsmServiceConfig service_config;
    service_config.m_self.m_app_type =
        WebAppConfig::to_string(m_config.m_web_app_config.m_interface);
    service_config.m_self.m_port          = m_config.m_port;
    service_config.m_self.m_is_controller = m_config.m_controller;
    service_config.m_app_name             = "hestia";

    std::unique_ptr<DistributedHsmService> hsm_service;
    if (m_config.m_controller) {
        hsm_service = DistributedHsmService::create(
            service_config, ApplicationContext::get().get_hsm_service(),
            ApplicationContext::get().get_kv_store_client());
    }
    else {
        hsm_service = DistributedHsmService::create(
            service_config, ApplicationContext::get().get_hsm_service(),
            ApplicationContext::get().get_http_client());
    }

    WebApp::Ptr web_app;
    std::unique_ptr<HsmS3Service> hsm_s3_service;

    if (m_config.m_web_app_config.m_interface == WebAppConfig::Interface::S3) {
        LOG_INFO("Running S3 interface");
        hsm_s3_service = std::make_unique<HsmS3Service>(
            ApplicationContext::get().get_hsm_service(),
            ApplicationContext::get().get_kv_store_client());

        HestiaS3WebAppConfig config;
        web_app =
            std::make_unique<HestiaS3WebApp>(config, hsm_s3_service.get());
    }
    else {
        LOG_INFO("Running http interface");
        web_app = std::make_unique<HestiaWebApp>(hsm_service.get());
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
