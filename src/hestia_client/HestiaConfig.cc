#include "HestiaConfig.h"

#include "YamlUtils.h"

#include <filesystem>

namespace hestia {

void HestiaConfig::load(const std::string& path)
{
    auto config_path = path;
    if (!std::filesystem::is_regular_file(config_path)) {
        config_path = std::filesystem::current_path() / "hestia.yaml";
    }

    if (!std::filesystem::is_regular_file(config_path)) {
        return;
    }

    Dictionary dict;
    YamlUtils::load(config_path, dict);

    const auto root = dict.get_map_item("root");
    load(*root);

    if (m_tier_backend_registry.empty()) {
        load_object_store_defaults();
    }
}

void HestiaConfig::load_server_config(const Dictionary& config)
{
    auto server_conf = config.get_map_item("server");
    if (server_conf == nullptr) {
        return;
    }

    Metadata sub_config;
    server_conf->get_map_items(sub_config);

    auto each_item = [this](const std::string& key, const std::string& value) {
        if (key == "host_address") {
            m_server_config.m_host = value;
        }
        else if (key == "host_port") {
            m_server_config.m_port = value;
        }
        else if (key == "web_app") {
            m_server_config.m_web_app = value;
        }
        else if (key == "backend") {
            m_server_config.m_backend = value;
        }
    };
    sub_config.for_each_item(each_item);
}

void HestiaConfig::load_defaults()
{
    load_object_store_defaults();
    load_kv_store_defaults();
}

void HestiaConfig::load_object_store_defaults()
{
    std::string default_root = m_cache_path + "/hsm_object_store";
    for (uint8_t idx = 0; idx < 5; idx++) {
        HsmObjectStoreClientSpec client_config(
            HsmObjectStoreClientSpec::Type::HSM,
            HsmObjectStoreClientSpec::Source::BUILT_IN,
            "hestia::FileHsmObjectStoreClient");
        client_config.m_extra_config.set_item("root", default_root);
        m_tier_backend_registry.emplace(idx, client_config);
    }
}

void HestiaConfig::load_kv_store_defaults()
{
    std::string default_root = m_cache_path + "/hsm_key_value_store";

    Metadata config;
    config.set_item("root", default_root);
    m_key_value_store_spec =
        KeyValueStoreClientSpec(KeyValueStoreClientSpec::Type::FILE, config);
}

void HestiaConfig::load_kv_store(const Dictionary& kv_store_config)
{
    (void)kv_store_config;
}

void HestiaConfig::load(const Dictionary& dict)
{
    if (auto cache_loc = dict.get_map_item("cache_location"); cache_loc) {
        m_cache_path = cache_loc->get_scalar();
    }

    auto object_store_config  = dict.get_map_item("object_store_clients");
    auto tier_client_registry = dict.get_map_item("tier_client_registry");

    if ((object_store_config != nullptr) && (tier_client_registry != nullptr)) {
        load_object_store_clients(*object_store_config, *tier_client_registry);
    }

    if (auto kv_store_spec = dict.get_map_item("key_value_store");
        kv_store_spec != nullptr) {
        load_kv_store(*kv_store_spec);
    }
    else {
        load_kv_store_defaults();
    }

    load_server_config(dict);
}

void HestiaConfig::load_object_store_clients(
    const Dictionary& object_store_config,
    const Dictionary& tier_client_registry)
{
    std::unordered_map<std::string, HsmObjectStoreClientSpec>
        object_store_clients;

    for (const auto& store : object_store_config.get_sequence()) {
        std::string identifier;
        if (const auto identifier_dict = store->get_map_item("identifier");
            identifier_dict != nullptr) {
            identifier = identifier_dict->get_scalar();
        }
        object_store_clients.emplace(
            identifier, HsmObjectStoreClientSpec(*store));
    }

    for (const auto& registry_entry : tier_client_registry.get_sequence()) {
        uint8_t identifier{0};
        if (const auto identifier_dict =
                registry_entry->get_map_item("identifier");
            identifier_dict != nullptr) {
            identifier =
                static_cast<uint8_t>(std::stoi(identifier_dict->get_scalar()));
        }

        std::string client_identifier;
        if (const auto identifier_dict =
                registry_entry->get_map_item("client_identifier");
            identifier_dict != nullptr) {
            client_identifier = identifier_dict->get_scalar();
        }

        if (auto iter = object_store_clients.find(client_identifier);
            iter != object_store_clients.end()) {
            m_tier_backend_registry.emplace(identifier, iter->second);
        }
    }
}
}  // namespace hestia