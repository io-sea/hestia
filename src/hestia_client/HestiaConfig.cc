#include "HestiaConfig.h"

#include "HsmObjectStoreClientManager.h"
#include "MultiBackendHsmObjectStoreClient.h"

#include "DataPlacementEngine.h"
#include "KeyValueStoreClient.h"

#include "ApplicationContext.h"
#include "HsmService.h"

#include "Logger.h"

namespace hestia {
int HestiaConfigurator::initialize(const HestiaConfig& config)
{
    m_config = config;

    std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store;
    try {
        object_store = set_up_object_store();
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return -1;
    }

    std::unique_ptr<KeyValueStore> kv_store;
    try {
        kv_store = set_up_key_value_store();
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return -1;
    }

    std::unique_ptr<DataPlacementEngine> dpe;
    try {
        dpe = set_up_data_placement_engine();
    }
    catch (const std::exception& e) {
        LOG_ERROR(e.what());
        return -1;
    }

    auto hsm_service = std::make_unique<HsmService>(
        std::move(kv_store), std::move(object_store), std::move(dpe));

    ApplicationContext::get().set_hsm_service(std::move(hsm_service));
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
        m_config.m_tier_backend_registry, m_config.m_copy_tool_config);
    return object_store;
}

std::unique_ptr<KeyValueStore> HestiaConfigurator::set_up_key_value_store()
{
    const auto store_type = m_config.m_key_value_store_type;
    if (!KeyValueStoreClientRegistry::is_store_type_available(store_type)) {
        std::string msg = "Requested Key Value Store type: "
                          + KeyValueStoreClientRegistry::to_string(store_type);
        msg += " is not available.";
        throw std::runtime_error(msg);
    }
    return KeyValueStoreClientRegistry::get_store(store_type);
}

std::unique_ptr<DataPlacementEngine>
HestiaConfigurator::set_up_data_placement_engine()
{
    // const auto pe_type = m_config.m_placement_engine_type;
    /*
    if (!placement_engine_registry::is_placement_engine_available(pe_type)) {
        std::string msg = "Requested Placement engine type: ";
        //                  + placement_engine_registry::to_string(pe_type);
        msg += " is not available.";
        throw std::runtime_error(msg);
    }
*/
    // return placement_engine_registry::get_engine(pe_type);
    return nullptr;
}
}  // namespace hestia