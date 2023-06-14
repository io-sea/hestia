#include "DistributedHsmObjectStoreClient.h"

#include "HsmObjectStoreClientFactory.h"
#include "HsmObjectStoreClientManager.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "Logger.h"
#include "TierService.h"

namespace hestia {
DistributedHsmObjectStoreClient::DistributedHsmObjectStoreClient(
    std::unique_ptr<HsmObjectStoreClientManager> client_manager) :
    HsmObjectStoreClient(), m_client_manager(std::move(client_manager))
{
}

DistributedHsmObjectStoreClient::~DistributedHsmObjectStoreClient() {}

DistributedHsmObjectStoreClient::Ptr DistributedHsmObjectStoreClient::create(
    const std::vector<std::filesystem::path>& plugin_paths)
{
    auto plugin_handler =
        std::make_unique<ObjectStorePluginHandler>(plugin_paths);
    auto client_factory = std::make_unique<HsmObjectStoreClientFactory>(
        std::move(plugin_handler));
    auto client_manager = std::make_unique<HsmObjectStoreClientManager>(
        std::move(client_factory));
    return std::make_unique<DistributedHsmObjectStoreClient>(
        std::move(client_manager));
}

void DistributedHsmObjectStoreClient::do_initialize(
    DistributedHsmService* hsm_service)
{
    m_hsm_service = hsm_service;

    std::unordered_map<std::string, HsmObjectStoreClientBackend> backends;
    for (const auto& backend :
         m_hsm_service->get_self_config().m_self.m_backends) {
        backends[backend.m_identifier] = backend;
    }

    auto response =
        m_hsm_service->get_hsm_service()->get_tier_service()->make_request(
            CrudMethod::MULTI_GET);
    std::unordered_map<uint8_t, StorageTier> tiers;
    for (const auto& tier : response->items()) {
        LOG_INFO("Adding tier: " << std::to_string(tier.id_uint()));
        tiers[tier.id_uint()] = tier;
    }

    m_client_manager->setup_clients(backends, tiers);
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request, Stream* stream) const noexcept
{
    if (request.method() == HsmObjectStoreRequestMethod::GET
        || request.method() == HsmObjectStoreRequestMethod::EXISTS) {
        if (m_client_manager->has_client(request.source_tier())) {
            if (m_client_manager->is_hsm_client(request.source_tier())) {
                auto client =
                    m_client_manager->get_hsm_client(request.source_tier());
                return client->make_request(request, stream);
            }
            else {
                auto client =
                    m_client_manager->get_client(request.source_tier());
                auto response = client->make_request(
                    HsmObjectStoreRequest::to_base_request(request), stream);
                return HsmObjectStoreResponse::create(
                    request, std::move(response));
            }
        }
        else {
            // Pull from remote
            return nullptr;
        }
    }
    else if (
        request.method() == HsmObjectStoreRequestMethod::PUT
        || request.method() == HsmObjectStoreRequestMethod::REMOVE) {
        if (m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Found client for target tier");
            if (m_client_manager->is_hsm_client(request.target_tier())) {
                auto client =
                    m_client_manager->get_hsm_client(request.target_tier());
                return client->make_request(request, stream);
            }
            else {
                auto client =
                    m_client_manager->get_client(request.target_tier());
                auto response = client->make_request(
                    HsmObjectStoreRequest::to_base_request(request), stream);
                return HsmObjectStoreResponse::create(
                    request, std::move(response));
            }
        }
        else {
            LOG_INFO("Did not find client for target tier - checking remotes");
            // push to remote
            return nullptr;
        }
    }
    else if (
        request.method() == HsmObjectStoreRequestMethod::COPY
        || request.method() == HsmObjectStoreRequestMethod::MOVE) {
        if (m_client_manager->has_client(request.source_tier())
            && m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Both clients for COPY/MOVE found");
            if (m_client_manager->have_same_client_types(
                    request.source_tier(), request.target_tier())) {
                LOG_INFO("Same clients handling COPY/MOVE");
                auto client =
                    m_client_manager->get_hsm_client(request.target_tier());
                return client->make_request(request);
            }
            else {
                LOG_INFO("Doing COPY/MOVE between different clients");
                // copy between clients
                return nullptr;
            }
        }
        else if (m_client_manager->has_client(request.source_tier())) {
            LOG_INFO("Only have source client for COPY/MOVE");
            // put to target tier via dist hsm service
            return nullptr;
        }
        else if (m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Only have target client for COPY/MOVE");
            // pull from source tier via dist hsm service
            return nullptr;
        }
        else {
            LOG_INFO("Don't have local clients for COPY/MOVE - try remote");
            // request move on different hsm service node
            return nullptr;
        }
    }
    LOG_INFO("Method not handled by dist object store client");
    return nullptr;
}
}  // namespace hestia
