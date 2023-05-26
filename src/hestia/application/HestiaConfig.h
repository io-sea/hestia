#pragma once

#include "Dictionary.h"

#include "CopyTool.h"
#include "DataPlacementEngineFactory.h"
#include "EventFeed.h"
#include "HsmObjectStoreClientFactory.h"
#include "KeyValueStoreClientFactory.h"
#include "Logger.h"
#include "ServerConfig.h"

#include <string>
#include <unordered_map>

namespace hestia {

class HestiaConfig {

  public:
    std::string load(const std::string& path);

    void load(const Dictionary& dict);

    void load_defaults();

    std::string m_cache_path{"hestia_cache"};

    ServerConfig m_server_config;

    // HSM
    TierBackendRegistry m_tier_backend_registry;
    KeyValueStoreClientSpec m_key_value_store_spec{
        KeyValueStoreClientSpec::Type::FILE};
    PlacementEngineType m_placement_engine_spec{
        hestia::PlacementEngineType::BASIC};

    CopyToolConfig m_copy_tool_config;

    EventFeedConfig m_event_feed_config;

  private:
    void load_logger_config();

    void load_object_store_clients(
        const Dictionary& object_store_clients,
        const Dictionary& tier_backend_registry);

    void load_kv_store(
        const Dictionary& kv_store_config, const Dictionary& kv_store_clients);

    void load_event_feed(const Dictionary& event_feed_config);

    void load_event_feed_defaults();

    void load_object_store_defaults();

    void load_kv_store_defaults();

    void load_server_config(const Dictionary& config);
};

}  // namespace hestia