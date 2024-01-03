#include "ObjectStoreBackendWithClient.h"

namespace hestia {

ObjectStoreBackendWithClient::ObjectStoreBackendWithClient(
    const ObjectStoreBackend& backend) :
    m_backend(backend)
{
}

ObjectStoreBackendWithClient::Ptr ObjectStoreBackendWithClient::create(
    const ObjectStoreBackend& backend)
{
    return std::make_unique<ObjectStoreBackendWithClient>(backend);
}

void ObjectStoreBackendWithClient::make_request(
    HsmObjectStoreContext& ctx) const noexcept
{
    if (m_backend.is_hsm()) {
        if (m_backend.is_built_in()) {
            m_hsm_object_store_client->make_request(ctx);
        }
        else {
            m_hsm_object_store_client_plugin->get_client()->make_request(ctx);
        }
    }
    else {
        auto object_store_ctx = ctx.to_object_store_context();
        if (m_backend.is_built_in()) {
            m_object_store_client->make_request(object_store_ctx);
        }
        else {
            m_object_store_client_plugin->get_client()->make_request(
                object_store_ctx);
        }
    }
}
}  // namespace hestia