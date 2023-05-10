#pragma once

#include "CopyTool.h"
#include "DataPlacementEngineRegistry.h"
#include "HsmObjectStoreClientRegistry.h"
#include "KeyValueStoreClientRegistry.h"

namespace hestia {

struct HestiaConfig {
    std::string m_host = "127.0.0.1";
    std::string m_port = "8080";
    TierBackendRegistry m_tier_backend_registry;
    KeyValueStoreType m_key_value_store_type{hestia::KeyValueStoreType::FILE};
    PlacementEngineType m_placement_engine_type{
        hestia::PlacementEngineType::BASIC};
    CopyToolConfig m_copy_tool_config;
};

}  // namespace hestia