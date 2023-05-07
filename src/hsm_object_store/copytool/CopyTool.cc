#include "CopyTool.h"

#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientManager.h"

#include "Stream.h"

namespace hestia {
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
}

HsmObjectStoreResponse::Ptr CopyTool::do_hsm_hsm(
    const HsmObjectStoreRequest& request,
    HsmObjectStoreClient* source_client,
    HsmObjectStoreClient* target_client) noexcept
{
    Stream stream;

    HsmObjectStoreRequest get_request = request;
    get_request.reset_method(HsmObjectStoreRequestMethod::GET);
    if (auto get_response = source_client->make_request(get_request, &stream);
        !get_response->ok()) {
        return get_response;
    }

    HsmObjectStoreRequest put_request = request;
    put_request.reset_method(HsmObjectStoreRequestMethod::PUT);
    if (auto put_response = target_client->make_request(put_request, &stream);
        !put_response->ok()) {
        return put_response;
    }

    auto response           = HsmObjectStoreResponse::create(request);
    const auto stream_state = stream.flush();
    if (!stream_state.ok()) {
        response->on_error(
            {HsmObjectStoreErrorCode::ERROR,
             "Error flushing stream: " + stream_state.message()});
    }
    return response;
}

HsmObjectStoreResponse::Ptr CopyTool::do_hsm_base(
    const HsmObjectStoreRequest& op,
    HsmObjectStoreClient* source_client,
    ObjectStoreClient* target_client) noexcept
{
    (void)op;
    (void)source_client;
    (void)target_client;

    return nullptr;
}

HsmObjectStoreResponse::Ptr CopyTool::do_base_hsm(
    const HsmObjectStoreRequest& op,
    ObjectStoreClient* source_client,
    HsmObjectStoreClient* target_client) noexcept
{
    (void)op;
    (void)source_client;
    (void)target_client;
    return nullptr;
}

HsmObjectStoreResponse::Ptr CopyTool::do_base_base(
    const HsmObjectStoreRequest& op,
    ObjectStoreClient* source_client,
    ObjectStoreClient* target_client) noexcept
{
    (void)op;
    (void)source_client;
    (void)target_client;
    return nullptr;
}
}  // namespace hestia