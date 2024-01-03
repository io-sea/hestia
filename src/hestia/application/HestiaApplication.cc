#include "HestiaApplication.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "HsmServicesFactory.h"

#include "EventFeed.h"
#include "EventSink.h"
#include "HsmEventSink.h"
#include "UserService.h"

#include "CurlClient.h"
#include "DistributedHsmObjectStoreClient.h"
#include "HsmObjectStoreClient.h"
#include "HttpClient.h"
#include "KeyValueStoreClient.h"
#include "S3Client.h"
#include "TypedCrudRequest.h"

#include "ProjectConfig.h"

#include "ErrorUtils.h"
#include "Logger.h"
#include "SystemUtils.h"
#include "TimeUtils.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace hestia {

#define DO_EXCEPT(msg)                                                         \
    LOG_ERROR(msg) throw std::runtime_error(SOURCE_LOC() + " | " + msg)

HestiaApplication::~HestiaApplication()
{
    Logger::get_instance().get_modifiable_context().m_config.set_active(false);
}

OpStatus HestiaApplication::initialize(
    const std::string& config_path,
    const std::string& user_token,
    const Dictionary& extra_config,
    const std::string& server_host,
    unsigned server_port)
{
    m_config.load(config_path, user_token, extra_config);

    initialize_logger();

    LOG_INFO(
        "Starting Hestia Version: " << project_config::get_project_version());

    if (m_config.get_config_path().empty()) {
        LOG_INFO("No config file found - using built-in defaults");
    }
    else {
        LOG_INFO("Using config at: " + m_config.get_config_path());
    }
    LOG_INFO("Using cache path: " + m_config.get_cache_path());

    set_app_mode(server_host, server_port);

    setup_http_clients();

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
            m_config.get_server_config().get_controller_address() + "/"
            + m_config.get_server_config().get_api_prefix();
    }

    setup_user_service(service_config, crud_backend.get());

    setup_hsm_service(service_config, crud_backend.get());

    const auto op_status =
        dynamic_cast<DistributedHsmObjectStoreClient*>(
            m_object_store_client.get())
            ->do_initialize(
                m_config.get_cache_path(), m_distributed_hsm_service.get());

    LOG_INFO("Finished Initializing Hestia");
    return op_status;
}

std::string HestiaApplication::get_runtime_info() const
{
    std::stringstream sstr;
    sstr << "Cache Location: " << m_config.get_cache_path() << '\n';
    sstr << "App Mode: "
         << ApplicationMode_enum_string_converter().init().to_string(m_app_mode)
         << '\n';
    return sstr.str();
}

const std::string& HestiaApplication::get_cache_path() const
{
    return m_config.get_cache_path();
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

void setup_tiers(
    CrudService* tier_service,
    const std::vector<StorageTier>& tiers,
    const CrudUserContext& user_context)
{
    if (tiers.empty()) {
        return;
    }

    LOG_INFO("Check for new Tiers to add.");
    CrudQuery query(CrudQuery::BodyFormat::ITEM);

    auto tiers_list_response = tier_service->make_request(
        CrudRequest{CrudMethod::READ, query, user_context});

    for (const auto& config_tier : tiers) {
        bool found{false};
        for (const auto& tier : tiers_list_response->items()) {
            if (tier->id() == config_tier.id()) {
                found = true;
                break;
            }
        }
        if (!found) {
            LOG_INFO("Adding tier: " << config_tier.id() << " to Tier service");
            if (auto response =
                    tier_service->make_request(TypedCrudRequest<StorageTier>{
                        CrudMethod::CREATE, config_tier, {}, user_context});
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

    hsm_config.m_self.set_is_controller(
        server_config.is_controller()
        || !server_config.has_controller_address());
    hsm_config.m_controller_address = server_config.get_controller_address();

    const auto host = server_config.get_host_address();
    const auto port = server_config.get_port();
    hsm_config.m_self.set_host_address(host);
    hsm_config.m_self.set_port(port);

    auto tag = server_config.get_tag();
    if (tag.empty()) {
        tag = "endpoint";
    }
    hsm_config.m_self.set_name(
        tag + "_" + SystemUtils::get_hostname().second + "_"
        + std::to_string(port));
}

void sync_configs(
    DistributedHsmServiceConfig& hsm_config, const HestiaConfig& config)
{
    sync_configs(hsm_config, config.get_server_config());
    hsm_config.m_backends = config.get_object_store_backends();

    hsm_config.m_self.set_version(project_config::get_project_version());
    hsm_config.m_app_name = project_config::get_project_name();
}

void HestiaApplication::setup_hsm_service(
    const ServiceConfig& config, CrudServiceBackend* backend)
{
    if (uses_local_storage()) {
        m_event_feed = std::make_unique<EventFeed>();
        m_event_feed->initialize(m_config.get_event_feed_config());
    }

    auto hsm_services = std::make_unique<HsmServiceCollection>();

    hsm_services->create_default_services(
        config, backend, m_user_service.get());

    if (m_config.default_dataset_enabled()) {
        hsm_services->get_service(HsmItem::Type::DATASET)
            ->set_default_name("hestia_default_dataset");
    }

    const auto current_user_context =
        m_user_service->get_current_user_context();

    if (uses_local_storage()) {
        setup_tiers(
            hsm_services->get_service(HsmItem::Type::TIER),
            m_config.get_storage_tiers(), current_user_context);
    }

    ServiceConfig hsm_service_config;
    auto hsm_service = std::make_unique<HsmService>(
        hsm_service_config, std::move(hsm_services),
        m_object_store_client.get(), m_event_feed.get());
    m_hsm_service = hsm_service.get();
    m_hsm_service->update_tiers(current_user_context);

    if (m_event_feed != nullptr && m_event_feed->is_active()) {
        const auto output_path =
            get_cache_path() + "/"
            + m_config.get_event_feed_config().get_output_path();
        auto event_sink =
            std::make_unique<HsmEventSink>(output_path, m_hsm_service);
        m_event_feed->add_sink(std::move(event_sink));
    }

    DistributedHsmServiceConfig service_config;
    sync_configs(service_config, m_config);
    service_config.m_is_server =
        m_app_mode == ApplicationMode::SERVER_WORKER
        || m_app_mode == ApplicationMode::SERVER_CONTROLLER;

    m_distributed_hsm_service = DistributedHsmService::create(
        service_config, std::move(hsm_service), m_user_service.get());

    m_distributed_hsm_service->register_self();
}

void HestiaApplication::setup_user_service(
    const ServiceConfig& config, CrudServiceBackend* backend)
{
    m_user_service = UserService::create(config, backend);

    LOG_INFO("Starting user log in");
    if (!m_config.get_user_token().empty()) {
        auto response =
            m_user_service->authenticate_with_token(m_config.get_user_token());
        if (!response->ok()) {
            const auto msg = "Failed to authenticate user.\n"
                             + response->get_error().to_string();
            DO_EXCEPT(msg);
        }
    }
    else {
        if (!m_config.user_management_enabled()) {
            auto response = m_user_service->load_or_create_default_user();
            if (!response->ok()) {
                const auto msg = "Failed to load or create default user.\n"
                                 + response->get_base_error().to_string();
                DO_EXCEPT(msg);
            }
        }
        else {
            throw std::runtime_error(
                "Failed to authenticate user - user token not provided");
        }
    }

    LOG_INFO(
        "Logged in with user: "
        << m_user_service->get_current_user().get_primary_key());
}

void HestiaApplication::setup_http_clients()
{
    CurlClientConfig http_client_config;
    m_http_client = std::make_unique<CurlClient>(http_client_config);
    m_s3_client   = std::make_unique<S3Client>(m_http_client.get());
}

void HestiaApplication::setup_object_store()
{
    std::vector<std::filesystem::path> search_paths = {
        project_config::get_hestia_plugin_path(), "/usr/lib/hestia",
        "/usr/lib64/hestia", "/usr/local/lib/hestia",
        "/usr/local/lib64/hestia"};
    m_object_store_client = DistributedHsmObjectStoreClient::create(
        m_http_client.get(), m_s3_client.get(), search_paths);
}

void HestiaApplication::setup_key_value_store()
{
    m_kv_store_client = KeyValueStoreClientFactory::get_client(
        m_config.get_cache_path(), m_config.get_key_value_store_config());
}

}  // namespace hestia