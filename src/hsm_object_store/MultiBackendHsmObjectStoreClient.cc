#include "MultiBackendHsmObjectStoreClient.h"

#include "CopyToolInterface.h"
#include "HsmObjectStoreClientManager.h"
#include "HsmObjectStoreClientRegistry.h"

namespace hestia {
MultiBackendHsmObjectStoreClient::MultiBackendHsmObjectStoreClient(
    std::unique_ptr<HsmObjectStoreClientManager> client_manager) :
    m_copy_tool_interface(std::make_unique<CopyToolInterface>()),
    m_client_manager(std::move(client_manager))
{
}

MultiBackendHsmObjectStoreClient::~MultiBackendHsmObjectStoreClient() {}

void MultiBackendHsmObjectStoreClient::initialize(
    const TierBackendRegistry& tier_backend_regsitry,
    const CopyToolConfig& copy_tool_config)
{
    m_client_manager->setup_clients(tier_backend_regsitry);
    m_copy_tool_interface->initialize(copy_tool_config);
}

HsmObjectStoreResponse::Ptr MultiBackendHsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request, Stream* stream) const noexcept
{
    if (request.is_hsm_only_request()) {
        if (m_client_manager->have_same_client_types(
                request.target_tier(), request.source_tier())) {
            if (m_client_manager->is_hsm_client(request.source_tier())) {
                auto client =
                    m_client_manager->get_hsm_client(request.source_tier());
                return client->make_request(request);
            }
            else {
                auto client =
                    m_client_manager->get_client(request.source_tier());
                auto response = client->make_request(
                    HsmObjectStoreRequest::to_base_request(request));
                return HsmObjectStoreResponse::create(
                    request, std::move(response));
            }
        }
        else {
            return HsmObjectStoreResponse::create(
                request, m_copy_tool_interface->make_request(request));
        }
    }
    else {
        const auto client_tier =
            request.method() == HsmObjectStoreRequestMethod::PUT ?
                request.target_tier() :
                request.source_tier();
        if (m_client_manager->is_hsm_client(client_tier)) {
            auto client = m_client_manager->get_hsm_client(client_tier);
            return client->make_request(request, stream);
        }
        else {
            auto client   = m_client_manager->get_client(client_tier);
            auto response = client->make_request(
                HsmObjectStoreRequest::to_base_request(request));
            return HsmObjectStoreResponse::create(request, std::move(response));
        }
    }
}
}  // namespace hestia
