#include "ObjectStoreClientManager.h"

#include "ErrorUtils.h"
#include "Logger.h"

#include <algorithm>

namespace hestia {
ObjectStoreClientManager::ObjectStoreClientManager(
    ObjectStoreClientFactory::Ptr client_factory) :
    m_client_factory(std::move(client_factory))
{
}

ObjectStoreBackendWithClient* ObjectStoreClientManager::get_backend(
    const std::string& tier_id) const
{
    if (auto backend_offset = get_backend_offset(tier_id);
        backend_offset.has_value()) {
        return m_backends[*backend_offset].get();
    }
    return nullptr;
}

std::optional<std::size_t> ObjectStoreClientManager::get_backend_offset(
    const std::string& tier_id) const
{
    if (const auto iter = m_tier_backend_offsets.find(tier_id);
        iter != m_tier_backend_offsets.end()) {
        if (iter->second.empty()) {
            return std::nullopt;
        }
        else {
            return iter->second[0];
        }
    }
    return std::nullopt;
}

bool ObjectStoreClientManager::has_backend(const std::string& tier_id) const
{
    return get_backend_offset(tier_id).has_value();
}

bool ObjectStoreClientManager::have_same_backend(
    const std::string& tier_id0, const std::string& tier_id1) const
{
    auto backend_idx0 = get_backend_offset(tier_id0);
    if (!backend_idx0.has_value()) {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Local backend for requested tier not found: " + tier_id0);
    }

    auto backend_idx1 = get_backend_offset(tier_id1);
    if (!backend_idx1.has_value()) {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Local backend for requested tier not found: " + tier_id1);
    }
    return backend_idx0 == backend_idx1;
}

void ObjectStoreClientManager::setup_clients(
    const ObjectStoreClientsConfig& config)
{
    initialize_backends(config);

    for (auto& backend_with_client : m_backends) {
        if (backend_with_client->m_backend.is_hsm()) {
            if (backend_with_client->m_backend.is_built_in()) {
                setup_built_in_hsm_client(backend_with_client.get(), config);
            }
            else {
                setup_plugin_hsm_client(backend_with_client.get(), config);
            }
        }
        else {
            if (backend_with_client->m_backend.is_built_in()) {
                setup_built_in_client(backend_with_client.get(), config);
            }
            else {
                setup_plugin_client(backend_with_client.get(), config);
            }
        }
    }
}

void ObjectStoreClientManager::initialize_backends(
    const ObjectStoreClientsConfig& config)
{
    // Map the backend key to its offset in the backend cache.
    std::unordered_map<std::string, std::size_t> backend_id_offsets;

    for (const auto& tier : config.m_tiers) {
        const auto tier_id = tier.get_primary_key();
        LOG_INFO(
            "Tier " << tier_id << " has " << tier.get_backends().size()
                    << " backends.");
        for (const auto& backend : tier.get_backends()) {
            // Backend is on this node
            if (backend.get_node_id() == config.m_node_id) {
                // If the backend client has already been created find its
                // offset
                if (auto iter =
                        backend_id_offsets.find(backend.get_primary_key());
                    iter != backend_id_offsets.end()) {
                    m_tier_backend_offsets[tier_id].push_back(iter->second);
                }
                else {
                    // Create a new client and store its offset
                    backend_id_offsets[backend.get_primary_key()] =
                        m_backends.size();
                    m_tier_backend_offsets[tier_id].push_back(
                        m_backends.size());
                    m_backends.push_back(
                        ObjectStoreBackendWithClient::create(backend));
                }
            }
        }
    }
}

void ObjectStoreClientManager::setup_built_in_client(
    ObjectStoreBackendWithClient* backend,
    const ObjectStoreClientsConfig& config)
{
    auto client =
        m_client_factory->get_client(backend->m_backend, config.m_s3_client);
    if (!client) {
        std::string msg = "Failed to retrieve client of type: "
                          + backend->m_backend.get_backend_as_string();
        LOG_ERROR(msg);
        throw std::runtime_error(SOURCE_LOC() + " | " + msg);
    }
    client->initialize(
        backend->m_backend.get_primary_key(), config.m_cache_path,
        backend->m_backend.get_config());
    backend->m_object_store_client = std::move(client);
}

void ObjectStoreClientManager::setup_built_in_hsm_client(
    ObjectStoreBackendWithClient* backend,
    const ObjectStoreClientsConfig& config)
{
    HsmObjectStoreClient::Ptr hsm_client;
    auto client =
        m_client_factory->get_client(backend->m_backend, config.m_s3_client);

    if (auto raw_client = dynamic_cast<HsmObjectStoreClient*>(client.get())) {
        client.release();
        auto hsm_client = std::unique_ptr<HsmObjectStoreClient>(raw_client);

        hsm_client->set_tier_ids(backend->m_backend.get_tier_ids());
        hsm_client->initialize(
            backend->m_backend.get_primary_key(), config.m_cache_path,
            backend->m_backend.get_config());
        backend->m_hsm_object_store_client = std::move(hsm_client);
    }
    else {
        throw std::runtime_error(
            SOURCE_LOC() + " | Failed to retrieve hsm client - bad cast.");
    }
}

void ObjectStoreClientManager::setup_plugin_client(
    ObjectStoreBackendWithClient* backend,
    const ObjectStoreClientsConfig& config)
{
    auto client_plugin =
        m_client_factory->get_client_from_plugin(backend->m_backend);
    client_plugin->get_client()->initialize(
        backend->m_backend.get_primary_key(), config.m_cache_path,
        backend->m_backend.get_config());
    backend->m_object_store_client_plugin = std::move(client_plugin);
}

void ObjectStoreClientManager::setup_plugin_hsm_client(
    ObjectStoreBackendWithClient* backend,
    const ObjectStoreClientsConfig& config)
{
    auto client_plugin =
        m_client_factory->get_hsm_client_from_plugin(backend->m_backend);
    client_plugin->get_client()->set_tier_ids(
        backend->m_backend.get_tier_ids());
    client_plugin->get_client()->initialize(
        backend->m_backend.get_primary_key(), config.m_cache_path,
        backend->m_backend.get_config());
    backend->m_hsm_object_store_client_plugin = std::move(client_plugin);
}

}  // namespace hestia
