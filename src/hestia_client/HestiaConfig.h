#pragma once

#include "Dictionary.h"

#include "CopyTool.h"
#include "DataPlacementEngineRegistry.h"
#include "HsmObjectStoreClientRegistry.h"
#include "KeyValueStoreClientRegistry.h"

#include <string>
#include <unordered_map>

namespace hestia {

class HestiaConfig {

  public:
    void load(const std::string& path);

    void load(const Dictionary& dict);

    void load_defaults();

    std::string m_host{"127.0.0.1"};
    std::string m_port{"8080"};
    std::string m_cache_path{"hestia_cache"};
    TierBackendRegistry m_tier_backend_registry;

    KeyValueStoreClientSpec m_key_value_store_spec{
        KeyValueStoreClientSpec::Type::FILE};
    PlacementEngineType m_placement_engine_spec{
        hestia::PlacementEngineType::BASIC};
    CopyToolConfig m_copy_tool_config;

  private:
    void load_object_store_clients(
        const Dictionary& object_store_clients,
        const Dictionary& tier_backend_registry);

    void load_kv_store(const Dictionary& kv_store_config);

    void load_object_store_defaults();

    void load_kv_store_defaults();
};

}  // namespace hestia