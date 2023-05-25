#include "HestiaConfigurator.h"

#include "HsmObjectStoreClientManager.h"
#include "MultiBackendHsmObjectStoreClient.h"

#include "DataPlacementEngine.h"
#include "EventFeed.h"
#include "KeyValueStoreClient.h"

#include "ApplicationContext.h"
#include "HsmService.h"
#include "ObjectService.h"
#include "TierService.h"

#include "Logger.h"
#include <memory>

namespace hestia {
OpStatus HestiaConfigurator::initialize(const HestiaConfig& config)
{
    m_config = config;

    std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store;
    try {
        object_store = set_up_object_store();
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return {OpStatus::Status::ERROR, -1, e.what()};
    }

    auto raw_object_store_client = object_store.get();
    ApplicationContext::get().set_object_store_client(std::move(object_store));

    std::unique_ptr<KeyValueStoreClient> kv_store_client;
    try {
        kv_store_client = set_up_key_value_store();
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return {OpStatus::Status::ERROR, -1, e.what()};
    }

    auto raw_kv_store_client = kv_store_client.get();
    ApplicationContext::get().set_kv_store_client(std::move(kv_store_client));

    TierServiceConfig tier_config;
    auto tier_service =
        std::make_unique<TierService>(tier_config, raw_kv_store_client);

    std::unique_ptr<DataPlacementEngine> dpe;
    try {
        dpe = set_up_data_placement_engine(tier_service.get());
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return {OpStatus::Status::ERROR, -1, e.what()};
    }

    ObjectServiceConfig object_config;
    auto object_service =
        std::make_unique<ObjectService>(object_config, raw_kv_store_client);

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
        raw_object_store_client, std::move(dpe), std::move(event_feed));

    ApplicationContext::get().set_hsm_service(std::move(hsm_service));
    return {};
}

std::unique_ptr<MultiBackendHsmObjectStoreClient>
HestiaConfigurator::set_up_object_store()
{
    LOG_INFO("Setting up object store");
    std::vector<std::filesystem::path> search_paths;
    auto plugin_handler =
        std::make_unique<ObjectStorePluginHandler>(search_paths);
    auto client_registry = std::make_unique<HsmObjectStoreClientFactory>(
        std::move(plugin_handler));
    auto client_manager = std::make_unique<HsmObjectStoreClientManager>(
        std::move(client_registry));

    auto object_store = std::make_unique<MultiBackendHsmObjectStoreClient>(
        std::move(client_manager));
    object_store->do_initialize(
        m_config.m_tier_backend_registry, m_config.m_copy_tool_config);
    return object_store;
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
    LOG_INFO("Setting up event feed");
    auto event_feed = std::make_unique<EventFeed>();

    event_feed->initialize(m_config.m_event_feed_config);

    return event_feed;
}
}  // namespace hestia