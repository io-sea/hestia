#include "HestiaConfigurator.h"

#include "DistributedHsmObjectStoreClient.h"
#include "HsmObjectStoreClientManager.h"

#include "DataPlacementEngine.h"
#include "EventFeed.h"

#include "CurlClient.h"
#include "HttpCrudClient.h"
#include "KeyValueCrudClient.h"
#include "KeyValueStoreClient.h"

#include "HttpObjectStoreClient.h"

#include "ApplicationContext.h"
#include "DistributedHsmService.h"
#include "HsmService.h"

#include "DatasetService.h"
#include "NamespaceService.h"
#include "ObjectService.h"
#include "TierService.h"
#include "UserService.h"

#include "ProjectConfig.h"

#include "Logger.h"
#include <memory>

namespace hestia {
OpStatus HestiaConfigurator::initialize(const HestiaConfig& config)
{
    m_config = config;
    LOG_INFO("Initializing Hestia with cache path: " + m_config.m_cache_path);

    CurlClientConfig http_client_config;
    auto http_client = std::make_unique<CurlClient>(http_client_config);
    ApplicationContext::get().set_http_client(std::move(http_client));

    auto http_object_store_client = std::make_unique<HttpObjectStoreClient>(
        ApplicationContext::get().get_http_client());

    std::vector<std::filesystem::path> search_paths;
    DistributedHsmObjectStoreClient::Ptr object_store;
    try {
        object_store = DistributedHsmObjectStoreClient::create(
            std::move(http_object_store_client), search_paths);
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return {OpStatus::Status::ERROR, -1, e.what()};
    }

    auto dist_object_store_client = object_store.get();
    ApplicationContext::get().set_object_store_client(std::move(object_store));

    std::unique_ptr<KeyValueStoreClient> kv_store_client;
    try {
        kv_store_client = set_up_key_value_store();
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return {OpStatus::Status::ERROR, -1, e.what()};
    }
    ApplicationContext::get().set_kv_store_client(std::move(kv_store_client));

    std::unique_ptr<TierService> tier_service;
    TierServiceConfig tier_service_config;
    tier_service_config.m_global_prefix = "hestia";
    if (m_config.m_server_config.m_controller) {
        tier_service = TierService::create(
            tier_service_config,
            ApplicationContext::get().get_kv_store_client());
        set_up_tiers(tier_service.get());
    }
    else {
        tier_service = TierService::create(
            tier_service_config, ApplicationContext::get().get_http_client());
    }

    std::unique_ptr<ObjectService> object_service;
    ObjectServiceConfig object_service_config;
    object_service_config.m_global_prefix = "hestia";
    if (m_config.m_server_config.m_controller) {
        object_service = ObjectService::create(
            object_service_config,
            ApplicationContext::get().get_kv_store_client());
    }
    else {
        object_service = ObjectService::create(
            object_service_config, ApplicationContext::get().get_http_client());
    }

    std::unique_ptr<DatasetService> dataset_service;
    DatasetServiceConfig dataset_service_config;
    dataset_service_config.m_global_prefix = "hestia";
    if (m_config.m_server_config.m_controller) {
        dataset_service = DatasetService::create(
            dataset_service_config,
            ApplicationContext::get().get_kv_store_client());
    }
    else {
        dataset_service = DatasetService::create(
            dataset_service_config,
            ApplicationContext::get().get_http_client());
    }

    std::unique_ptr<NamespaceService> namespace_service;
    NamespaceServiceConfig namespace_service_config;
    namespace_service_config.m_global_prefix = "hestia";
    if (m_config.m_server_config.m_controller) {
        namespace_service = NamespaceService::create(
            namespace_service_config,
            ApplicationContext::get().get_kv_store_client());
    }
    else {
        namespace_service = NamespaceService::create(
            namespace_service_config,
            ApplicationContext::get().get_http_client());
    }
    ApplicationContext::get().set_namespace_service(
        std::move(namespace_service));

    std::unique_ptr<UserService> user_service;
    UserServiceConfig user_service_config;
    user_service_config.m_global_prefix = "hestia";
    if (m_config.m_server_config.m_controller) {
        user_service = UserService::create(
            user_service_config,
            ApplicationContext::get().get_kv_store_client());
    }
    else {
        user_service = UserService::create(
            user_service_config, ApplicationContext::get().get_http_client());
    }
    ApplicationContext::get().set_user_service(std::move(user_service));

    std::unique_ptr<DataPlacementEngine> dpe;
    try {
        dpe = set_up_data_placement_engine(tier_service.get());
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return {OpStatus::Status::ERROR, -1, e.what()};
    }

    std::unique_ptr<EventFeed> event_feed;
    try {
        event_feed = set_up_event_feed();
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return {OpStatus::Status::ERROR, -1, e.what()};
    }

    auto hsm_service = std::make_unique<HsmService>(
        std::move(object_service), std::move(tier_service),
        std::move(dataset_service),
        ApplicationContext::get().get_object_store_client(), std::move(dpe),
        std::move(event_feed));


    DistributedHsmServiceConfig service_config;
    service_config.m_self.m_app_type = WebAppConfig::to_string(
        m_config.m_server_config.m_web_app_config.m_interface);
    service_config.m_self.m_port         = m_config.m_server_config.m_port;
    service_config.m_self.m_host_address = m_config.m_server_config.m_host;
    service_config.m_self.m_is_controller =
        m_config.m_server_config.m_controller;

    for (const auto& [id, backend] : m_config.m_backends) {
        service_config.m_self.m_backends.push_back(backend);
    }

    std::string tag = m_config.m_server_config.m_tag;
    if (tag.empty()) {
        tag = "endpoint";
    }
    service_config.m_self.set_name(
        tag + "_" + m_config.m_server_config.m_host + "_"
        + m_config.m_server_config.m_port);
    service_config.m_self.m_version = project_config::get_project_version();
    service_config.m_app_name       = project_config::get_project_name();
    service_config.m_controller_address =
        m_config.m_server_config.m_controller_address;

    std::unique_ptr<DistributedHsmService> dist_hsm_service;
    if (m_config.m_server_config.m_controller) {
        dist_hsm_service = DistributedHsmService::create(
            service_config, std::move(hsm_service),
            ApplicationContext::get().get_kv_store_client());
    }
    else {
        dist_hsm_service = DistributedHsmService::create(
            service_config, std::move(hsm_service),
            ApplicationContext::get().get_http_client());
    }

    ApplicationContext::get().set_hsm_service(std::move(dist_hsm_service));

    dist_object_store_client->do_initialize(
        ApplicationContext::get().get_hsm_service());
    return {};
}

bool HestiaConfigurator::set_up_tiers(TierService* tier_service)
{
    if (m_config.m_tiers.empty()) {
        return true;
    }

    LOG_INFO("Check for new Tiers to add.");

    auto tiers_list_response =
        tier_service->make_request(CrudMethod::MULTI_GET);
    for (const auto& [id, tier] : m_config.m_tiers) {

        bool found{false};
        for (const auto& tier : tiers_list_response->items()) {
            if (tier.name() == std::to_string(id)) {
                found = true;
                break;
            }
        }

        if (!found) {
            LOG_INFO(
                "Adding tier: " << std::to_string(id) << " to Tier service");

            CrudRequest<StorageTier> request(tier, CrudMethod::PUT);
            request.set_generate_id(true);
            auto response = tier_service->make_request(request);
            if (!response->ok()) {
                LOG_ERROR("Failed to PUT tier in initialization.");
                return false;
            }
        }
    }
    return true;
}

std::unique_ptr<KeyValueStoreClient>
HestiaConfigurator::set_up_key_value_store()
{
    LOG_INFO("Setting up key-value store");
    if (!KeyValueStoreClientFactory::is_client_type_available(
            m_config.m_key_value_store_spec)) {
        std::string msg = "Requested Key Value Store type: "
                          + KeyValueStoreClientFactory::to_string(
                              m_config.m_key_value_store_spec);
        msg += " is not available.";
        throw std::runtime_error(msg);
    }
    return KeyValueStoreClientFactory::get_client(
        m_config.m_key_value_store_spec);
}

std::unique_ptr<DataPlacementEngine>
HestiaConfigurator::set_up_data_placement_engine(TierService* tier_service)
{
    LOG_INFO("Setting up data placement engine");
    if (!DataPlacementEngineFactory::is_placement_engine_available(
            m_config.m_placement_engine_spec)) {
        std::string msg = "Requested Placement engine type: ";
        msg += " is not available.";
        throw std::runtime_error(msg);
    }

    return DataPlacementEngineFactory::get_engine(
        m_config.m_placement_engine_spec, tier_service);
}

std::unique_ptr<EventFeed> HestiaConfigurator::set_up_event_feed()
{
    auto event_feed = std::make_unique<EventFeed>();
    event_feed->initialize(m_config.m_cache_path, m_config.m_event_feed_config);

    return event_feed;
}
}  // namespace hestia