#include "HsmObjectStoreClientManager.h"

namespace hestia {
HsmObjectStoreClientManager::HsmObjectStoreClientManager(
    HsmObjectStoreClientRegistry::Ptr client_registry) :
    m_client_registry(std::move(client_registry))
{
}

hestia::ObjectStoreClient* HsmObjectStoreClientManager::get_client(
    ObjectStoreClientType client_type) const
{
    if (const auto iter = m_hsm_clients.find(client_type.m_identifier);
        iter != m_hsm_clients.end()) {
        return iter->second.get();
    }
    else {
        if (const auto iter = m_clients.find(client_type.m_identifier);
            iter != m_clients.end()) {
            return iter->second.get();
        }
        else {
            return nullptr;
        }
    }
}

HsmObjectStoreClient* HsmObjectStoreClientManager::get_hsm_client(
    ObjectStoreClientType client_type) const
{
    if (!client_type.is_hsm()) {
        throw std::runtime_error(
            "Requested HSM client for non-hsm client type.");
    }

    if (const auto iter = m_hsm_clients.find(client_type.m_identifier);
        iter != m_hsm_clients.end()) {
        return iter->second.get();
    }
    else {
        return nullptr;
    }
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
        if (!m_client_registry->is_client_type_available(item.second)) {
            std::string msg = "Client " + item.second.m_identifier;
            msg += " not available. Requested for tier id: "
                   + std::to_string(item.first);
            throw std::runtime_error(msg);
        }

        if (item.second.is_hsm()) {
            if (get_hsm_client(item.second) == nullptr) {
                HsmObjectStoreClient::Ptr hsm_client;
                auto client = m_client_registry->get_client(item.second);

                if (auto raw_client =
                        dynamic_cast<HsmObjectStoreClient*>(client.get())) {
                    client.release();
                    auto hsm_client =
                        std::unique_ptr<HsmObjectStoreClient>(raw_client);
                    m_hsm_clients[item.second.m_identifier] =
                        std::move(hsm_client);
                }
                else {
                    throw std::runtime_error(
                        "Failed to retreive hsm client - bad cast.");
                }
            }
        }
        else if (get_client(item.second) == nullptr) {
            m_clients[item.second.m_identifier] =
                m_client_registry->get_client(item.second);
        }
    }
}
}  // namespace hestia