#include "HsmObjectStoreClientManager.h"

#include "Logger.h"

#include <algorithm>

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
    return get_hsm_client(get_backend(tier_id)) != nullptr;
}

ObjectStoreClient* HsmObjectStoreClientManager::get_client(
    uint8_t tier_id) const
{
    if (const auto iter = m_tier_backends.find(tier_id);
        iter != m_tier_backends.end()) {
        return get_client(iter->second);
    }
    else {
        return nullptr;
    }
}

std::string HsmObjectStoreClientManager::get_backend(uint8_t tier_id) const
{
    if (const auto iter = m_tier_backends.find(tier_id);
        iter != m_tier_backends.end()) {
        return iter->second;
    }
    else {
        return {};
    }
}

HsmObjectStoreClient* HsmObjectStoreClientManager::get_hsm_client(
    uint8_t tier_id) const
{
    if (const auto iter = m_tier_backends.find(tier_id);
        iter != m_tier_backends.end()) {
        return get_hsm_client(iter->second);
    }
    else {
        return nullptr;
    }
}

bool HsmObjectStoreClientManager::has_client(uint8_t tier_id) const
{
    const auto backend = get_backend(tier_id);
    if (backend.empty()) {
        return false;
    }
    return has_backend(backend);
}

bool HsmObjectStoreClientManager::has_backend(const std::string& backend) const
{
    return std::find(m_backends.begin(), m_backends.end(), backend)
           != m_backends.end();
}

bool HsmObjectStoreClientManager::have_same_client_types(
    uint8_t tier_id0, uint8_t tier_id1) const
{
    auto tier0_iter = m_tier_backends.find(tier_id0);
    if (tier0_iter == m_tier_backends.end()) {
        throw std::runtime_error("Requested tier not found");
    }

    auto tier1_iter = m_tier_backends.find(tier_id1);
    if (tier1_iter == m_tier_backends.end()) {
        throw std::runtime_error("Requested tier not found");
    }
    return tier0_iter->second == tier1_iter->second;
}

void HsmObjectStoreClientManager::setup_clients(
    const std::string& cache_path,
    const std::vector<HsmObjectStoreClientBackend>& backends,
    const std::vector<StorageTier>& tiers)
{
    LOG_INFO("Got: " << backends.size() << " backends");

    for (const auto& tier : tiers) {
        m_tier_backends[tier.id_uint()] = tier.get_backend();
    }

    for (const auto& backend : backends) {
        const auto identifier = backend.get_identifier();
        m_backends.push_back(identifier);

        LOG_INFO("Processing backend: " + identifier);

        if (!m_client_factory->is_client_type_available(backend)) {
            std::string msg = "Client " + identifier + " not available.";
            throw std::runtime_error(msg);
        }

        if (backend.is_hsm()) {
            if (get_hsm_client(identifier) == nullptr) {
                LOG_INFO("Setting up hsm client of type: " + identifier);

                if (backend.is_built_in()) {
                    HsmObjectStoreClient::Ptr hsm_client;
                    auto client = m_client_factory->get_client(backend);

                    if (auto raw_client =
                            dynamic_cast<HsmObjectStoreClient*>(client.get())) {
                        client.release();
                        auto hsm_client =
                            std::unique_ptr<HsmObjectStoreClient>(raw_client);

                        hsm_client->initialize(
                            cache_path, backend.get_config());
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
                        cache_path, backend.get_config());
                    m_hsm_plugin_clients[identifier] = std::move(client_plugin);
                }
            }
        }
        else if (get_client(identifier) == nullptr) {
            LOG_INFO("Setting up client of type: " + identifier);

            if (backend.is_built_in()) {
                auto client = m_client_factory->get_client(backend);
                client->initialize(cache_path, backend.get_config());
                if (!client) {
                    throw std::runtime_error(
                        "Failed to retrieve client of type: " + identifier);
                }
                m_clients[identifier] = std::move(client);
            }
            else {
                auto client_plugin =
                    m_client_factory->get_client_from_plugin(backend);
                client_plugin->get_client()->initialize(
                    cache_path, backend.get_config());
                m_plugin_clients[identifier] = std::move(client_plugin);
            }
        }
    }
}
}  // namespace hestia
