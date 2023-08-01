#include "HestiaConfig.h"

#include "HestiaCache.h"
#include "Logger.h"
#include "YamlUtils.h"

#include <cstdlib>
#include <filesystem>

namespace hestia {

HestiaConfig::HestiaConfig() : SerializeableWithFields("hestia_config")
{
    init();
}

HestiaConfig::HestiaConfig(const HestiaConfig& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

HestiaConfig& HestiaConfig::operator=(const HestiaConfig& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_path          = other.m_path;
        m_user_token    = other.m_user_token;
        m_cache_path    = other.m_cache_path;
        m_server_config = other.m_server_config;

        m_logger                 = other.m_logger;
        m_key_value_store_config = other.m_key_value_store_config;
        m_backends               = other.m_backends;
        m_tiers                  = other.m_tiers;
        m_event_feed_config      = other.m_event_feed_config;

        m_enable_user_management = other.m_enable_user_management;
        m_enable_default_dataset = other.m_enable_default_dataset;
        init();
    }
    return *this;
}

void HestiaConfig::init()
{
    register_scalar_field(&m_cache_path);
    register_scalar_field(&m_enable_user_management);
    register_scalar_field(&m_enable_default_dataset);
    register_map_field(&m_server_config);

    register_map_field(&m_logger);
    register_map_field(&m_key_value_store_config);
    register_sequence_field(&m_backends);
    register_sequence_field(&m_tiers);
    register_map_field(&m_event_feed_config);
}

void HestiaConfig::add_object_store_backend(
    const HsmObjectStoreClientBackend& backend)
{
    m_backends.get_container_as_writeable().push_back(backend);
}

void HestiaConfig::add_storage_tier(const StorageTier& tier)
{
    m_tiers.get_container_as_writeable().push_back(tier);
}

const EventFeedConfig& HestiaConfig::get_event_feed_config() const
{
    return m_event_feed_config.value();
}

const KeyValueStoreClientConfig& HestiaConfig::get_key_value_store_config()
    const
{
    return m_key_value_store_config.value();
}

const std::string& HestiaConfig::get_user_token() const
{
    return m_user_token;
}

const LoggerConfig& HestiaConfig::get_logger_config() const
{
    return m_logger.value();
}

const std::vector<HsmObjectStoreClientBackend>&
HestiaConfig::get_object_store_backends() const
{
    return m_backends.container();
}

const std::vector<StorageTier>& HestiaConfig::get_storage_tiers() const
{
    return m_tiers.container();
}

bool HestiaConfig::default_dataset_enabled() const
{
    return m_enable_default_dataset.get_value();
}

bool HestiaConfig::user_management_enabled() const
{
    return m_enable_user_management.get_value();
}

const std::string& HestiaConfig::get_config_path() const
{
    return m_path;
}

void HestiaConfig::find_config_file(const std::string& config_path)
{
    if (!config_path.empty() && std::filesystem::is_regular_file(config_path)) {
        m_path = config_path;
        return;
    }

    if (!m_path.empty() && std::filesystem::is_regular_file(m_path)) {
        return;
    }

    if (auto config_path_env = std::getenv("HESTIA_CONFIG_PATH");
        config_path_env != nullptr) {
        const auto check_path = std::string(config_path_env);
        if (std::filesystem::is_regular_file(check_path)) {
            m_path = check_path;
            return;
        }
    }

    if (auto home_dir = std::getenv("HOME"); home_dir != nullptr) {
        const std::string home_str =
            std::string(home_dir) + "/.config/hestia/hestia.yaml";
        if (std::filesystem::is_regular_file(home_str)) {
            m_path = home_str;
        }
    }
}

void HestiaConfig::find_user_token(const std::string& user_token)
{
    if (user_token.empty()) {
        if (auto user_token_env = std::getenv("HESTIA_USER_TOKEN");
            user_token_env != nullptr) {
            m_user_token = std::string(user_token_env);
        }
    }
    else {
        m_user_token = user_token;
    }
}

void HestiaConfig::load(
    const std::string& config_path,
    const std::string& user_token,
    const Dictionary& extra_config)
{
    find_config_file(config_path);

    if (m_path.empty()) {
        load_from_dict(extra_config, user_token);
    }
    else {
        Dictionary dict;
        YamlUtils::load(m_path, dict);

        const auto root = dict.get_map_item("root");
        root->merge(extra_config);
        load_from_dict(*root, user_token);
    }
}

void HestiaConfig::load_from_dict(
    const Dictionary& dict, const std::string& user_token)
{
    find_user_token(user_token);
    deserialize(dict);

    if (m_cache_path.get_value().empty()) {
        m_cache_path.update_value(HestiaCache::get_default_cache_dir());
    }
}

void HestiaConfig::override_controller_address(
    const std::string& host, unsigned port)
{
    m_server_config.get_value_as_writeable().set_controller_address(host, port);
}

const std::string& HestiaConfig::get_cache_path() const
{
    return m_cache_path.get_value();
}

bool HestiaConfig::has_object_store_backends() const
{
    return !m_backends.container().empty();
}

const ServerConfig& HestiaConfig::get_server_config() const
{
    return m_server_config.value();
}

}  // namespace hestia