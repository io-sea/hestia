#include "TestClientConfigs.h"

#include "InMemoryHsmObjectStoreClient.h"
#include "KeyValueStoreClientFactory.h"
#include "Logger.h"
#include "ObjectStoreBackend.h"
#include "StorageTier.h"

namespace hestia {
void TestClientConfigs::get_hsm_memory_client_config(Dictionary& config)
{
    LoggerConfig logger_config;
    logger_config.set_console_only(true);
    logger_config.set_active(true);

    auto logger_config_dict = std::make_unique<Dictionary>();
    logger_config.serialize(*logger_config_dict);

    KeyValueStoreClientConfig kv_config;
    kv_config.set_client_type(KeyValueStoreClientConfig::Type::MEMORY);

    auto kv_config_dict = std::make_unique<Dictionary>();
    kv_config.serialize(*kv_config_dict);

    auto tiers = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    std::vector<std::string> tier_ids;
    for (std::size_t idx = 0; idx < 5; idx++) {
        StorageTier tier(std::to_string(idx + 1));
        tier.set_priority(idx);
        auto tier_dict = std::make_unique<Dictionary>();
        tier.serialize(*tier_dict);
        tier_ids.push_back(tier.id());
        tiers->add_sequence_item(std::move(tier_dict));
    }

    ObjectStoreBackend object_store_config(
        ObjectStoreBackend::Type::MEMORY_HSM);
    object_store_config.set_tier_ids(tier_ids);

    InMemoryObjectStoreClientConfig in_memory_store_config;

    Dictionary in_memory_store_config_dict;
    in_memory_store_config.serialize(in_memory_store_config_dict);
    object_store_config.set_config(in_memory_store_config_dict);

    auto object_store_configs =
        std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    auto object_store_dict = std::make_unique<Dictionary>();
    object_store_config.serialize(*object_store_dict);
    object_store_configs->add_sequence_item(std::move(object_store_dict));

    config.set_map_item(
        LoggerConfig::get_type(), std::move(logger_config_dict));
    config.set_map_item(
        KeyValueStoreClientConfig::get_type(), std::move(kv_config_dict));
    config.set_map_item(
        ObjectStoreBackend::get_type() + "s", std::move(object_store_configs));
    config.set_map_item(StorageTier::get_type() + "s", std::move(tiers));
}
}  // namespace hestia