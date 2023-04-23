#include "CopyTool.h"

#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientManager.h"

#include <ostk/Stream.h>

CopyTool::CopyTool(HsmObjectStoreClientManager::Ptr client_manager) :
    m_client_manager(std::move(client_manager))
{
}

CopyTool::~CopyTool() {}

void CopyTool::initialize() {}

HsmObjectStoreResponse::Ptr CopyTool::make_object_store_request(
    const HsmObjectStoreRequest& request) noexcept
{
    if (m_client_manager->is_hsm_client(request.source_tier())) {
        if (m_client_manager->is_hsm_client(request.target_tier())) {
            return do_hsm_hsm(
                request,
                m_client_manager->get_hsm_client(request.source_tier()),
                m_client_manager->get_hsm_client(request.target_tier()));
        }
        else {
            return do_hsm_base(
                request,
                m_client_manager->get_hsm_client(request.source_tier()),
                m_client_manager->get_client(request.target_tier()));
        }
    }
    else {
        if (m_client_manager->is_hsm_client(request.target_tier())) {
            return do_base_hsm(
                request, m_client_manager->get_client(request.source_tier()),
                m_client_manager->get_hsm_client(request.target_tier()));
        }
        else {
            return do_base_base(
                request, m_client_manager->get_client(request.source_tier()),
                m_client_manager->get_client(request.target_tier()));
        }
    }
};

HsmObjectStoreResponse::Ptr CopyTool::do_hsm_hsm(
    const HsmObjectStoreRequest& request,
    HsmObjectStoreClient* source_client,
    HsmObjectStoreClient* target_client) noexcept
{
    std::size_t chunk_size{4000};
    ostk::Stream stream;

    HsmObjectStoreRequest get_request = request;
    get_request.resetMethod(HsmObjectStoreRequestMethod::GET);
    if (auto get_response = source_client->make_request(get_request, &stream);
        !get_response->ok()) {
        return get_response;
    }

    HsmObjectStoreRequest put_request = request;
    put_request.resetMethod(HsmObjectStoreRequestMethod::PUT);
    if (auto put_response = target_client->make_request(put_request, &stream);
        !put_response->ok()) {
        return put_response;
    }

    auto response           = HsmObjectStoreResponse::create(request);
    const auto stream_state = stream.flush();
    if (!stream_state.ok()) {
        response->onError(
            {HsmObjectStoreErrorCode::ERROR,
             "Error flushing stream: " + stream_state.message()});
    }
    return response;
}

HsmObjectStoreResponse::Ptr CopyTool::do_hsm_base(
    const HsmObjectStoreRequest& op,
    HsmObjectStoreClient* source_client,
    ostk::ObjectStoreClient* target_client) noexcept
{
    return nullptr;
}

HsmObjectStoreResponse::Ptr CopyTool::do_base_hsm(
    const HsmObjectStoreRequest& op,
    ostk::ObjectStoreClient* source_client,
    HsmObjectStoreClient* target_client) noexcept
{
    return nullptr;
}

HsmObjectStoreResponse::Ptr CopyTool::do_base_base(
    const HsmObjectStoreRequest& op,
    ostk::ObjectStoreClient* source_client,
    ostk::ObjectStoreClient* target_client) noexcept
{
    return nullptr;
}