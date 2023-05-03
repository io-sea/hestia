#pragma once

#include "CopyTool.h"
#include "DataPlacementEngineRegistry.h"
#include "HsmObjectStoreClientRegistry.h"
#include "KeyValueStoreClientRegistry.h"

class DataPlacementEngine;

namespace hestia {
class MultiBackendHsmObjectStoreClient;
class KeyValueStore;

struct HestiaConfig {
    TierBackendRegistry m_tier_backend_registry;
    KeyValueStoreType m_key_value_store_type;
    PlacementEngineType m_placement_engine_type;
    CopyToolConfig m_copy_tool_config;
};

class HestiaConfigurator {
  public:
    int initialize(const HestiaConfig& config);

  private:
    std::unique_ptr<MultiBackendHsmObjectStoreClient> set_up_object_store();

    std::unique_ptr<KeyValueStore> set_up_key_value_store();

    std::unique_ptr<DataPlacementEngine> set_up_data_placement_engine();

    HestiaConfig m_config;
};
}  // namespace hestia