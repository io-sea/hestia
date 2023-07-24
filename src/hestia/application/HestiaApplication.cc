#include "HestiaApplication.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "HsmServicesFactory.h"

#include "UserService.h"

#include "CurlClient.h"
#include "DistributedHsmObjectStoreClient.h"
#include "HsmObjectStoreClient.h"
#include "HttpClient.h"
#include "HttpObjectStoreClient.h"
#include "KeyValueStoreClient.h"
#include "TypedCrudRequest.h"

#include "ProjectConfig.h"

#include "Logger.h"
#include "TimeUtils.h"

#include <iostream>

namespace hestia {

HestiaApplication::~HestiaApplication() {}

OpStatus HestiaApplication::initialize(
    const std::string& config_path,
    const std::string& user_token,
    const Dictionary& extra_config)
{
    m_config.load(config_path, user_token, extra_config);

    initialize_logger();

    LOG_INFO(
        "Starting Hestia Version: " << project_config::get_project_version());
    LOG_INFO("Using cache path: " + m_config.get_cache_path());

    set_app_mode();

    if (uses_http_client()) {
        setup_http_client();
    }

    setup_object_store();

    std::unique_ptr<CrudServiceBackend> crud_backend;
    if (uses_local_storage()) {
        setup_key_value_store();
        crud_backend = std::make_unique<KeyValueStoreCrudServiceBackend>(
            m_kv_store_client.get());
    }
    else {
        crud_backend =
            std::make_unique<HttpRestCrudServiceBackend>(m_http_client.get());
    }

    ServiceConfig service_config;
    service_config.m_global_prefix = "hestia";
    if (!uses_local_storage()) {
        service_config.m_endpoint =
            m_config.get_server_config().get_controller_address();
    }

    setup_user_service(service_config, crud_backend.get());

    setup_hsm_service(service_config, crud_backend.get());

    dynamic_cast<DistributedHsmObjectStoreClient*>(m_object_store_client.get())
        ->do_initialize(
            m_config.get_cache_path(), m_distributed_hsm_service.get());

    LOG_INFO("Finished Initializing Hestia");
    return {};
}

void HestiaApplication::initialize_logger() const
{
    Logger::get_instance().do_initialize(
        m_config.get_cache_path(), m_config.get_logger_config());
}

bool HestiaApplication::uses_local_storage() const
{
    return m_app_mode == ApplicationMode::CLIENT_STANDALONE
           || m_app_mode == ApplicationMode::SERVER_CONTROLLER;
}

bool HestiaApplication::uses_http_client() const
{
    return m_app_mode != ApplicationMode::CLIENT_STANDALONE;
}

void setup_tiers(
    CrudService* tier_service, const std::vector<StorageTier>& tiers)
{
    if (tiers.empty()) {
        return;
    }

    LOG_INFO("Check for new Tiers to add.");
    CrudQuery query(CrudQuery::OutputFormat::ITEM);

    auto tiers_list_response = tier_service->make_request(CrudRequest{query});

    for (const auto& config_tier : tiers) {
        bool found{false};
        for (const auto& tier : tiers_list_response->items()) {
            if (tier->name() == config_tier.name()) {
                found = true;
                break;
            }
        }
        if (!found) {
            LOG_INFO(
                "Adding tier: " << config_tier.name() << " to Tier service");
            if (auto response =
                    tier_service->make_request(TypedCrudRequest<StorageTier>{
                        CrudMethod::CREATE, config_tier});
                !response->ok()) {
                LOG_ERROR("Failed to PUT tier in initialization.");
                return;
            }
        }
    }
}

void sync_configs(
    DistributedHsmServiceConfig& hsm_config, const ServerConfig& server_config)
{
    hsm_config.m_self.set_app_type(
        server_config.get_web_app_config().get_interface_as_string());

    hsm_config.m_self.set_is_controller(server_config.is_controller());
    hsm_config.m_controller_address = server_config.get_controller_address();

    const auto host = server_config.get_host_address();
    const auto port = server_config.get_port();
    hsm_config.m_self.set_host_address(host);
    hsm_config.m_self.set_port(port);

    auto tag = server_config.get_tag();
    if (tag.empty()) {
        tag = "endpoint";
    }
    hsm_config.m_self.set_name(tag + "_" + host + "_" + std::to_string(port));
}

void sync_configs(
    DistributedHsmServiceConfig& hsm_config, const HestiaConfig& config)
{
    sync_configs(hsm_config, config.get_server_config());

    hsm_config.m_self.set_backends(config.get_object_store_backends());
    hsm_config.m_self.set_version(project_config::get_project_version());
    hsm_config.m_app_name = project_config::get_project_name();
}

void HestiaApplication::setup_hsm_service(
    const ServiceConfig& config, CrudServiceBackend* backend)
{
    auto hsm_services = std::make_unique<HsmServiceCollection>();

    hsm_services->create_default_services(
        config, backend, m_user_service.get());

    if (uses_local_storage()) {
        setup_tiers(
            hsm_services->get_service(HsmItem::Type::TIER),
            m_config.get_storage_tiers());
    }

    auto dpe = DataPlacementEngineFactory::get_engine(
        PlacementEngineType::BASIC,
        hsm_services->get_service(HsmItem::Type::TIER));

    auto event_feed = std::make_unique<EventFeed>();
    event_feed->initialize(
        m_config.get_cache_path(), m_config.get_event_feed_config());

    ServiceConfig hsm_service_config;
    auto hsm_service = std::make_unique<HsmService>(
        hsm_service_config, std::move(hsm_services),
        m_object_store_client.get(), std::move(dpe), std::move(event_feed));
    m_hsm_service = hsm_service.get();

    DistributedHsmServiceConfig service_config;
    sync_configs(service_config, m_config);

    m_distributed_hsm_service = DistributedHsmService::create(
        service_config, std::move(hsm_service), backend, m_user_service.get());

    if (m_app_mode == ApplicationMode::SERVER_WORKER) {
        m_distributed_hsm_service->register_self();
    }
}

void HestiaApplication::setup_user_service(
    const ServiceConfig& config, CrudServiceBackend* backend)
{
    m_user_service = UserService::create(config, backend);
    if (!m_config.get_user_token().empty()) {
        m_user_service->authenticate_with_token(m_config.get_user_token());
    }
}

void HestiaApplication::setup_http_client()
{
    CurlClientConfig http_client_config;
    m_http_client = std::make_unique<CurlClient>(http_client_config);
}

void HestiaApplication::setup_object_store()
{
    std::vector<std::filesystem::path> search_paths;
    std::unique_ptr<HttpObjectStoreClient> http_object_store_client;
    if (uses_http_client()) {
        http_object_store_client =
            std::make_unique<HttpObjectStoreClient>(m_http_client.get());
    }

    m_object_store_client = DistributedHsmObjectStoreClient::create(
        std::move(http_object_store_client), search_paths);
}

void HestiaApplication::setup_key_value_store()
{
    m_kv_store_client = KeyValueStoreClientFactory::get_client(
        m_config.get_cache_path(), m_config.get_key_value_store_config());
}

}  // namespace hestia