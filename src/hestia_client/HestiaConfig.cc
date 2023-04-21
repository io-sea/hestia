#include "HestiaConfig.h"

#include "HsmObjectStoreClientManager.h"
#include "MultiBackendHsmObjectStoreClient.h"

#include "DataPlacementEngine.h"
#include "KeyValueStoreClient.h"

#include "ApplicationContext.h"
#include "HsmService.h"

#include <iostream>

namespace hestia {
int HestiaConfigurator::initialize(const HestiaConfig& config)
{
    m_config = config;

    std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store;
    try {
        object_store = setUpObjectStore();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    std::unique_ptr<KeyValueStore> kv_store;
    try {
        kv_store = setUpKeyValueStore();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    std::unique_ptr<DataPlacementEngine> dpe;
    try {
        dpe = setUpDataPlacementEngine();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }

    auto hsm_service = std::make_unique<HsmService>(
        std::move(kv_store), std::move(object_store), std::move(dpe));

    ApplicationContext::get().setHsmService(std::move(hsm_service));
    return 0;
}

std::unique_ptr<MultiBackendHsmObjectStoreClient>
HestiaConfigurator::set_up_object_store()
{
    std::vector<std::filesystem::directory_entry> search_paths;
    auto plugin_handler =
        std::make_unique<ObjectStorePluginHandler>(search_paths);
    auto client_registry = std::make_unique<HsmObjectStoreClientRegistry>(
        std::move(plugin_handler));
    auto client_manager = std::make_unique<HsmObjectStoreClientManager>(
        std::move(client_registry));

    auto object_store = std::make_unique<MultiBackendHsmObjectStoreClient>(
        std::move(client_manager));
    object_store->initialize(
        m_config.m_tier_backend_registry, mConfig.m_copy_tool_config);
    return object_store;
}

std::unique_ptr<KeyValueStore> HestiaConfigurator::set_up_key_value_store()
{
    const auto store_type = m_config.m_key_value_store_type;
    if (!KeyValueStoreRegistry::isStoreTypeAvailable(store_type)) {
        std::string msg = "Requested Key Value Store type: "
                          + KeyValueStoreRegistry::toString(store_type);
        msg += " is not available.";
        throw std::runtime_error(msg);
    }
    return KeyValueStoreRegistry::getStore(store_type);
}

std::unique_ptr<DataPlacementEngine>
HestiaConfigurator::set_up_data_placement_engine()
{
    const auto pe_type = m_config.m_placement_engine_type;
    if (!PlacementEngineRegistry::isPlacementEngineAvailable(pe_type)) {
        std::string msg = "Requested Placement engine type: "
                          + PlacementEngineRegistry::toString(pe_type);
        msg += " is not available.";
        throw std::runtime_error(msg);
    }

    return PlacementEngineRegistry::getEngine(pe_type);
}
}  // namespace hestia