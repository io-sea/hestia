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
    ObjectStoreBackend::Type identifier) const
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
    ObjectStoreBackend::Type identifier) const
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

ObjectStoreBackend::Type HsmObjectStoreClientManager::get_backend(
    uint8_t tier_id) const
{
    if (const auto iter = m_tier_backends.find(tier_id);
        iter != m_tier_backends.end()) {
        return iter->second;
    }
    else {
        return ObjectStoreBackend::Type::UNKNOWN;
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
    if (backend == ObjectStoreBackend::Type::UNKNOWN) {
        return false;
    }
    return has_backend(backend);
}

bool HsmObjectStoreClientManager::has_backend(
    ObjectStoreBackend::Type backend) const
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
    const std::string& node_id,
    const std::vector<StorageTier>& tiers,
    const std::vector<ObjectStoreBackend>& local_backends)
{
    LOG_INFO("Got: " << local_backends.size() << " backends");

    ObjectStoreBackend::Type_enum_string_converter converter;
    converter.init();

    if (node_id.empty()) {
        LOG_INFO("Setting up clients in standalone mode");
        for (const auto& backend : local_backends) {
            for (const auto& tier : tiers) {
                LOG_INFO("Tier id is: " << tier.get_primary_key());
                for (const auto& id : backend.get_tier_ids()) {
                    LOG_INFO("Backend tier id is: " << id);
                    if (id == tier.get_primary_key()) {
                        LOG_INFO(
                            "Registering backend type: "
                            << backend.get_backend_as_string());
                        m_tier_backends[tier.id_uint()] = backend.get_backend();
                        break;
                    }
                }
            }
        }
    }
    else {
        for (const auto& tier : tiers) {
            LOG_INFO("Tier id is: " << tier.get_primary_key());
            for (const auto& backend : tier.get_backends()) {
                LOG_INFO("Backend id is: " << backend.get_primary_key());
                if (backend.get_node_id() == node_id) {
                    LOG_INFO(
                        "Backend is on this node - registering it for tier");
                    m_tier_backends[tier.id_uint()] = backend.get_backend();
                }
            }
        }
    }

    for (const auto& backend : local_backends) {
        const auto identifier = backend.get_backend();
        m_backends.push_back(identifier);

        if (!m_client_factory->is_client_type_available(backend)) {
            std::string msg =
                "Client " + converter.to_string(identifier) + " not available.";
            throw std::runtime_error(msg);
        }

        if (backend.is_hsm()) {
            if (get_hsm_client(identifier) == nullptr) {
                if (backend.is_built_in()) {
                    HsmObjectStoreClient::Ptr hsm_client;
                    auto client = m_client_factory->get_client(backend);

                    if (auto raw_client =
                            dynamic_cast<HsmObjectStoreClient*>(client.get())) {
                        client.release();
                        auto hsm_client =
                            std::unique_ptr<HsmObjectStoreClient>(raw_client);

                        hsm_client->set_tier_names(backend.get_tier_names());
                        hsm_client->initialize(
                            backend.get_primary_key(), cache_path,
                            backend.get_config());
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
                    client_plugin->get_client()->set_tier_names(
                        backend.get_tier_names());
                    client_plugin->get_client()->initialize(
                        backend.get_primary_key(), cache_path,
                        backend.get_config());
                    m_hsm_plugin_clients[identifier] = std::move(client_plugin);
                }
            }
        }
        else if (get_client(identifier) == nullptr) {
            if (backend.is_built_in()) {
                auto client = m_client_factory->get_client(backend);
                client->initialize(
                    backend.get_primary_key(), cache_path,
                    backend.get_config());
                if (!client) {
                    throw std::runtime_error(
                        "Failed to retrieve client of type: "
                        + converter.to_string(identifier));
                }
                m_clients[identifier] = std::move(client);
            }
            else {
                auto client_plugin =
                    m_client_factory->get_client_from_plugin(backend);
                LOG_INFO("Intializing plugin");
                client_plugin->get_client()->initialize(
                    backend.get_primary_key(), cache_path,
                    backend.get_config());
                m_plugin_clients[identifier] = std::move(client_plugin);
            }
        }
    }
}
}  // namespace hestia
