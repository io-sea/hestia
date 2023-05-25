#pragma once

#include "HsmMiddlewareResponse.h"
#include "HsmObjectStoreError.h"
#include "HsmObjectStoreRequest.h"

#include "ObjectStoreResponse.h"

namespace hestia {
class HsmObjectStoreResponse :
    public hestia::BaseObjectStoreResponse<HsmObjectStoreErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreResponse>;

    HsmObjectStoreResponse(const BaseObjectStoreRequest& request);
    HsmObjectStoreResponse(
        const BaseObjectStoreRequest& request,
        HsmMiddlewareResponse::Ptr middleware_response);
    HsmObjectStoreResponse(
        const BaseObjectStoreRequest& request,
        HsmObjectStoreResponse::Ptr hsm_child_response);
    HsmObjectStoreResponse(
        const BaseObjectStoreRequest& request,
        ObjectStoreResponse::Ptr child_response);

    static Ptr create(const BaseObjectStoreRequest& request);
    static Ptr create(
        const BaseObjectStoreRequest& request,
        HsmMiddlewareResponse::Ptr middleware_response);
    static Ptr create(
        const BaseObjectStoreRequest& request,
        HsmObjectStoreResponse::Ptr hsm_child_response);
    static Ptr create(
        const BaseObjectStoreRequest& request,
        ObjectStoreResponse::Ptr child_response);

    static ObjectStoreResponse::Ptr to_base_response(
        const BaseObjectStoreRequest& request,
        const HsmObjectStoreResponse* response);

  private:
    HsmMiddlewareResponse::Ptr m_middleware_response;
    HsmObjectStoreResponse::Ptr m_hsm_child_response;
    ObjectStoreResponse::Ptr m_child_response;
};
}  // namespace hestia