#include "TierService.h"

#include "KeyValueStoreClient.h"
#include "Metadata.h"

#include "JsonUtils.h"

namespace hestia {

TierService::TierService(
    const TierServiceConfig& config, KeyValueStoreClient* kv_store_client) :
    m_config(config), m_kv_store_client(kv_store_client)
{
}

std::string TierService::get_key_prefix() const
{
    return m_config.m_key_prefix + ":" + m_config.m_tier_key;
}

void TierService::add_tier(const StorageTier& tier) const
{
    const std::string key = get_key_prefix() + ":" + tier.m_id;

    Metadata metadata;
    metadata.set_item("id", tier.m_id);
    metadata.set_item("backends", tier.m_backend);

    const Metadata::Query query{key, JsonUtils::to_json(metadata)};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::STRING_SET, query);
    const auto response = m_kv_store_client->make_request(request);
    if (!response->ok()) {
        throw std::runtime_error(
            "Error in kv_store STRING_SET: "
            + response->get_error().to_string());
    }

    const std::string set_key = get_key_prefix() + "s";
    const Metadata::Query set_add_query{set_key, tier.m_id};
    KeyValueStoreRequest set_request(
        KeyValueStoreRequestMethod::SET_ADD, set_add_query);
    const auto set_response = m_kv_store_client->make_request(set_request);
    if (!set_response->ok()) {
        throw std::runtime_error(
            "Error in kv_store SET_ADD: " + response->get_error().to_string());
    }
}

void TierService::add_tiers(const std::vector<StorageTier>& tiers) const
{
    for (const auto& tier : tiers) {
        add_tier(tier);
    }
}

void TierService::get_tiers(std::vector<StorageTier>& tiers) const
{
    const std::string key = get_key_prefix() + "s";
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::SET_LIST, query);
    const auto response = m_kv_store_client->make_request(request);
    if (!response->ok()) {
        throw std::runtime_error(
            "Error in kv_store SET_LIST: " + response->get_error().to_string());
    }

    for (const auto& item : response->get_set_items()) {
        tiers.push_back(StorageTier(item));
    }
}

void TierService::get_tier(uint8_t tier_id, StorageTier& tier) const
{
    const std::string key = get_key_prefix() + ":" + std::to_string(tier_id);
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::STRING_GET, query);
    const auto response = m_kv_store_client->make_request(request);

    Metadata metadata;
    JsonUtils::from_json(response->get_value(), metadata);

    auto on_item = [&tier](const std::string& key, const std::string& value) {
        if (key == "backend") {
            tier.m_backend = value;
        }
        else if (key == "id") {
            tier.m_id = value;
        }
    };
    metadata.for_each_item(on_item);
}
}  // namespace hestia