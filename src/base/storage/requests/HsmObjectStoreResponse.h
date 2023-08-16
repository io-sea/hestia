#pragma once

#include "HsmObjectStoreError.h"
#include "HsmObjectStoreRequest.h"

#include "ObjectStoreResponse.h"

namespace hestia {
class HsmObjectStoreResponse :
    public hestia::BaseObjectStoreResponse<HsmObjectStoreErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreResponse>;

    HsmObjectStoreResponse(
        const BaseObjectStoreRequest& request, const std::string& client_id);
    HsmObjectStoreResponse(
        const BaseObjectStoreRequest& request,
        HsmObjectStoreResponse::Ptr hsm_child_response);
    HsmObjectStoreResponse(
        const BaseObjectStoreRequest& request,
        ObjectStoreResponse::Ptr child_response);

    static Ptr create(
        const BaseObjectStoreRequest& request, const std::string& client_id);
    static Ptr create(
        const BaseObjectStoreRequest& request,
        HsmObjectStoreResponse::Ptr hsm_child_response);
    static Ptr create(
        const BaseObjectStoreRequest& request,
        ObjectStoreResponse::Ptr child_response);

    static ObjectStoreResponse::Ptr to_base_response(
        const BaseObjectStoreRequest& request,
        const HsmObjectStoreResponse* response);

    void set_is_handled_remote(bool handled_remote)
    {
        m_handled_remote = handled_remote;
    }

    bool is_handled_remote() const { return m_handled_remote; }

  private:
    HsmObjectStoreResponse::Ptr m_hsm_child_response;
    ObjectStoreResponse::Ptr m_child_response;
    bool m_handled_remote{false};
};
}  // namespace hestia