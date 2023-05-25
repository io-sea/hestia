#include "HsmObjectStoreClientManager.h"

#include "Logger.h"

namespace hestia {
HsmObjectStoreClientManager::HsmObjectStoreClientManager(
    HsmObjectStoreClientFactory::Ptr client_factory) :
    m_client_factory(std::move(client_factory))
{
}

hestia::ObjectStoreClient* HsmObjectStoreClientManager::get_client(
    const HsmObjectStoreClientSpec& client_spec) const
{
    if (const auto iter = m_hsm_clients.find(client_spec.m_identifier);
        iter != m_hsm_clients.end()) {
        return iter->second.get();
    }
    else if (const auto iter = m_clients.find(client_spec.m_identifier);
             iter != m_clients.end()) {
        return iter->second.get();
    }
    else if (const auto iter = m_plugin_clients.find(client_spec.m_identifier);
             iter != m_plugin_clients.end()) {
        return iter->second.get()->get_client();
    }
    else if (const auto iter =
                 m_hsm_plugin_clients.find(client_spec.m_identifier);
             iter != m_hsm_plugin_clients.end()) {
        return iter->second.get()->get_client();
    }
    return nullptr;
}

HsmObjectStoreClient* HsmObjectStoreClientManager::get_hsm_client(
    const HsmObjectStoreClientSpec& client_spec) const
{
    if (!client_spec.is_hsm()) {
        throw std::runtime_error(
            "Requested HSM client for non-hsm client type.");
    }

    if (const auto iter = m_hsm_clients.find(client_spec.m_identifier);
        iter != m_hsm_clients.end()) {
        return iter->second.get();
    }
    else if (const auto iter =
                 m_hsm_plugin_clients.find(client_spec.m_identifier);
             iter != m_hsm_plugin_clients.end()) {
        return iter->second.get()->get_client();
    }
    return nullptr;
}

bool HsmObjectStoreClientManager::is_hsm_client(uint8_t tier_id) const
{
    if (const auto iter = m_tier_backend_registry.find(tier_id);
        iter != m_tier_backend_registry.end()) {
        return iter->second.is_hsm();
    }
    return false;
}

ObjectStoreClient* HsmObjectStoreClientManager::get_client(
    uint8_t tier_id) const
{
    if (const auto iter = m_tier_backend_registry.find(tier_id);
        iter != m_tier_backend_registry.end()) {
        return get_client(iter->second);
    }
    else {
        return nullptr;
    }
}

HsmObjectStoreClient* HsmObjectStoreClientManager::get_hsm_client(
    uint8_t tier_id) const
{
    if (const auto iter = m_tier_backend_registry.find(tier_id);
        iter != m_tier_backend_registry.end()) {
        return get_hsm_client(iter->second);
    }
    else {
        return nullptr;
    }
}

bool HsmObjectStoreClientManager::have_same_client_types(
    uint8_t tier_id0, uint8_t tier_id1) const
{
    auto type0 = m_tier_backend_registry.find(tier_id0)->second;
    auto type1 = m_tier_backend_registry.find(tier_id1)->second;
    return type0.m_identifier == type1.m_identifier;
}

void HsmObjectStoreClientManager::setup_clients(
    const TierBackendRegistry& tier_backend_regsitry)
{
    m_tier_backend_registry = tier_backend_regsitry;

    for (const auto& item : m_tier_backend_registry) {
        LOG_INFO(
            "Processing tier-backend entry: " + std::to_string(item.first)
            + " | " + item.second.m_identifier);

        if (!m_client_factory->is_client_type_available(item.second)) {
            std::string msg = "Client " + item.second.m_identifier;
            msg += " not available. Requested for tier id: "
                   + std::to_string(item.first);
            throw std::runtime_error(msg);
        }

        if (item.second.is_hsm()) {
            if (get_hsm_client(item.second) == nullptr) {
                LOG_INFO(
                    "Setting up hsm client of type: "
                    + item.second.to_string());

                if (item.second.m_source
                    == HsmObjectStoreClientSpec::Source::BUILT_IN) {
                    HsmObjectStoreClient::Ptr hsm_client;
                    auto client = m_client_factory->get_client(item.second);

                    if (auto raw_client =
                            dynamic_cast<HsmObjectStoreClient*>(client.get())) {
                        client.release();
                        auto hsm_client =
                            std::unique_ptr<HsmObjectStoreClient>(raw_client);

                        hsm_client->initialize(item.second.m_extra_config);
                        m_hsm_clients[item.second.m_identifier] =
                            std::move(hsm_client);
                    }
                    else {
                        throw std::runtime_error(
                            "Failed to retrieve hsm client - bad cast.");
                    }
                }
                else {
                    auto client_plugin =
                        m_client_factory->get_hsm_client_from_plugin(
                            item.second);
                    client_plugin->get_client()->initialize(
                        item.second.m_extra_config);
                    m_hsm_plugin_clients[item.second.m_identifier] =
                        std::move(client_plugin);
                }
            }
        }
        else if (get_client(item.second) == nullptr) {
            LOG_INFO("Setting up client of type: " + item.second.to_string());

            if (item.second.m_source
                == HsmObjectStoreClientSpec::Source::BUILT_IN) {
                auto client = m_client_factory->get_client(item.second);
                client->initialize(item.second.m_extra_config);
                if (!client) {
                    throw std::runtime_error(
                        "Failed to retrieve client of type: "
                        + item.second.to_string());
                }
                m_clients[item.second.m_identifier] = std::move(client);
            }
            else {
                auto client_plugin =
                    m_client_factory->get_client_from_plugin(item.second);
                client_plugin->get_client()->initialize(
                    item.second.m_extra_config);
                m_plugin_clients[item.second.m_identifier] =
                    std::move(client_plugin);
            }
        }
    }
}
}  // namespace hestia