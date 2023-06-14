#include "HsmObjectStoreClientManager.h"

#include "Logger.h"

namespace hestia {
HsmObjectStoreClientManager::HsmObjectStoreClientManager(
    HsmObjectStoreClientFactory::Ptr client_factory) :
    m_client_factory(std::move(client_factory))
{
}

hestia::ObjectStoreClient* HsmObjectStoreClientManager::get_client(
    const std::string& identifier) const
{
    if (const auto iter = m_hsm_clients.find(identifier);
        iter != m_hsm_clients.end()) {
        return iter->second.get();
    }
    else if (const auto iter = m_clients.find(identifier);
             iter != m_clients.end()) {
        return iter->second.get();
    }
    else if (const auto iter = m_plugin_clients.find(identifier);
             iter != m_plugin_clients.end()) {
        return iter->second.get()->get_client();
    }
    else if (const auto iter = m_hsm_plugin_clients.find(identifier);
             iter != m_hsm_plugin_clients.end()) {
        return iter->second.get()->get_client();
    }
    return nullptr;
}

HsmObjectStoreClient* HsmObjectStoreClientManager::get_hsm_client(
    const std::string& identifier) const
{
    if (auto iter = m_backends.find(identifier); iter != m_backends.end()) {
        if (!iter->second.is_hsm()) {
            throw std::runtime_error(
                "Requested HSM client for non-hsm client type.");
        }
    }
    else {
        throw std::runtime_error("Requested backend not found.");
    }

    if (const auto iter = m_hsm_clients.find(identifier);
        iter != m_hsm_clients.end()) {
        return iter->second.get();
    }
    else if (const auto iter = m_hsm_plugin_clients.find(identifier);
             iter != m_hsm_plugin_clients.end()) {
        return iter->second.get()->get_client();
    }
    return nullptr;
}

bool HsmObjectStoreClientManager::is_hsm_client(uint8_t tier_id) const
{
    if (const auto iter = m_tiers.find(tier_id); iter != m_tiers.end()) {
        if (const auto backend_iter = m_backends.find(iter->second.m_backend);
            backend_iter != m_backends.end()) {
            return backend_iter->second.is_hsm();
        }
    }
    return false;
}

ObjectStoreClient* HsmObjectStoreClientManager::get_client(
    uint8_t tier_id) const
{
    if (const auto iter = m_tiers.find(tier_id); iter != m_tiers.end()) {
        return get_client(iter->second.m_backend);
    }
    else {
        return nullptr;
    }
}

HsmObjectStoreClient* HsmObjectStoreClientManager::get_hsm_client(
    uint8_t tier_id) const
{
    if (const auto iter = m_tiers.find(tier_id); iter != m_tiers.end()) {
        return get_hsm_client(iter->second.m_backend);
    }
    else {
        return nullptr;
    }
}

bool HsmObjectStoreClientManager::has_client(uint8_t tier_id) const
{
    if (const auto iter = m_tiers.find(tier_id); iter != m_tiers.end()) {
        if (const auto backend_iter = m_backends.find(iter->second.m_backend);
            backend_iter != m_backends.end()) {
            return true;
        }
    }
    return false;
}

bool HsmObjectStoreClientManager::have_same_client_types(
    uint8_t tier_id0, uint8_t tier_id1) const
{
    auto type0 = m_tiers.find(tier_id0)->second;
    auto type1 = m_tiers.find(tier_id1)->second;
    return type0.m_backend == type1.m_backend;
}

void HsmObjectStoreClientManager::setup_clients(
    const std::unordered_map<std::string, HsmObjectStoreClientBackend>&
        backends,
    const std::unordered_map<uint8_t, StorageTier>& tiers)
{
    m_tiers = tiers;
    for (const auto& [id, tier] : m_tiers) {
        LOG_INFO(
            "Adding tier: " << std::to_string(id)
                            << " with backend: " << tier.m_backend);
    }

    m_backends = backends;

    for (const auto& [identifier, backend] : m_backends) {
        LOG_INFO("Processing backend: " + identifier);

        if (!m_client_factory->is_client_type_available(backend)) {
            std::string msg = "Client " + identifier + " not available.";
            throw std::runtime_error(msg);
        }

        if (backend.is_hsm()) {
            if (get_hsm_client(identifier) == nullptr) {
                LOG_INFO("Setting up hsm client of type: " + identifier);

                if (backend.m_source
                    == HsmObjectStoreClientBackend::Source::BUILT_IN) {
                    HsmObjectStoreClient::Ptr hsm_client;
                    auto client = m_client_factory->get_client(backend);

                    if (auto raw_client =
                            dynamic_cast<HsmObjectStoreClient*>(client.get())) {
                        client.release();
                        auto hsm_client =
                            std::unique_ptr<HsmObjectStoreClient>(raw_client);

                        hsm_client->initialize(backend.m_extra_config);
                        m_hsm_clients[identifier] = std::move(hsm_client);
                    }
                    else {
                        throw std::runtime_error(
                            "Failed to retrieve hsm client - bad cast.");
                    }
                }
                else {
                    auto client_plugin =
                        m_client_factory->get_hsm_client_from_plugin(backend);
                    client_plugin->get_client()->initialize(
                        backend.m_extra_config);
                    m_hsm_plugin_clients[identifier] = std::move(client_plugin);
                }
            }
        }
        else if (get_client(identifier) == nullptr) {
            LOG_INFO("Setting up client of type: " + identifier);

            if (backend.m_source
                == HsmObjectStoreClientBackend::Source::BUILT_IN) {
                auto client = m_client_factory->get_client(backend);
                client->initialize(backend.m_extra_config);
                if (!client) {
                    throw std::runtime_error(
                        "Failed to retrieve client of type: " + identifier);
                }
                m_clients[identifier] = std::move(client);
            }
            else {
                auto client_plugin =
                    m_client_factory->get_client_from_plugin(backend);
                client_plugin->get_client()->initialize(backend.m_extra_config);
                m_plugin_clients[identifier] = std::move(client_plugin);
            }
        }
    }
}
}  // namespace hestia