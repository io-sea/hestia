#pragma once

#include "DataPlacementEngineFactory.h"
#include "EventFeed.h"
#include "HsmObjectStoreClientBackend.h"
#include "KeyValueStoreClientFactory.h"
#include "ServerConfig.h"

#include "Dictionary.h"

namespace hestia {

class HestiaConfig {

  public:
    std::string load(const std::string& path);

    void load(const Dictionary& dict);

    void load_defaults();

    std::string m_cache_path{"hestia_cache"};

    ServerConfig m_server_config;

    std::unordered_map<std::string, HsmObjectStoreClientBackend> m_backends;
    std::unordered_map<uint8_t, StorageTier> m_tiers;

    KeyValueStoreClientSpec m_key_value_store_spec{
        KeyValueStoreClientSpec::Type::FILE};
    PlacementEngineType m_placement_engine_spec{
        hestia::PlacementEngineType::BASIC};

    EventFeedConfig m_event_feed_config;

  private:
    void load_logger_config();

    void load_object_store_clients(
        const Dictionary& backends, const Dictionary& tiers);

    void load_kv_store(
        const Dictionary& kv_store_config, const Dictionary& kv_store_clients);

    void load_event_feed(const Dictionary& event_feed_config);

    void load_event_feed_defaults();

    void load_object_store_defaults();

    void load_kv_store_defaults();

    void load_server_config(const Dictionary& config);
};

}  // namespace hestia