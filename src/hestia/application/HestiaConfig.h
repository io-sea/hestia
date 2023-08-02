#pragma once

#include "DataPlacementEngineFactory.h"
#include "EventFeed.h"
#include "KeyValueStoreClientFactory.h"
#include "LoggerConfig.h"
#include "ObjectStoreBackend.h"
#include "SerializeableWithFields.h"
#include "ServerConfig.h"

#include "StorageTier.h"

#include "Dictionary.h"

namespace hestia {

class HestiaConfig : public SerializeableWithFields {
  public:
    HestiaConfig();

    HestiaConfig(const HestiaConfig& other);

    void add_object_store_backend(const ObjectStoreBackend& backend);

    void add_storage_tier(const StorageTier& tier);

    const EventFeedConfig& get_event_feed_config() const;

    const KeyValueStoreClientConfig& get_key_value_store_config() const;

    const LoggerConfig& get_logger_config() const;

    const ServerConfig& get_server_config() const;

    const std::string& get_cache_path() const;

    const std::string& get_config_path() const;

    const std::vector<ObjectStoreBackend>& get_object_store_backends() const;

    const std::vector<StorageTier>& get_storage_tiers() const;

    const std::string& get_user_token() const;

    bool user_management_enabled() const;

    bool default_dataset_enabled() const;

    bool has_object_store_backends() const;

    void load(
        const std::string& config_path = {},
        const std::string& user_token  = {},
        const Dictionary& extra_config = {});

    void load_from_dict(
        const Dictionary& dict, const std::string& user_token = {});

    void override_controller_address(const std::string& host, unsigned port);

    HestiaConfig& operator=(const HestiaConfig& other);

  private:
    void init();

    void find_config_file(const std::string& config_path);

    void find_user_token(const std::string& user_token);

    std::string m_path;
    std::string m_user_token;
    StringField m_cache_path{"cache_path"};
    BooleanField m_enable_user_management{"enable_user_management", false};
    BooleanField m_enable_default_dataset{"enable_default_dataset", true};

    TypedDictField<ServerConfig> m_server_config{ServerConfig::get_type()};

    TypedDictField<LoggerConfig> m_logger{LoggerConfig::get_type()};
    TypedDictField<KeyValueStoreClientConfig> m_key_value_store_config{
        KeyValueStoreClientConfig::get_type()};
    SequenceField<std::vector<ObjectStoreBackend>> m_backends{
        std::string(ObjectStoreBackend::get_type()) + "s"};
    SequenceField<std::vector<StorageTier>> m_tiers{
        std::string(HsmItem::tier_name) + "s"};
    TypedDictField<EventFeedConfig> m_event_feed_config{
        EventFeedConfig::get_type()};
};

}  // namespace hestia