#pragma once

#include "HsmMiddlewareResponse.h"
#include "HsmObjectStoreError.h"
#include "HsmObjectStoreRequest.h"

#include <ostk/ObjectStoreResponse.h>

class HsmObjectStoreResponse :
    public ostk::BaseObjectStoreResponse<HsmObjectStoreErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreResponse>;

    HsmObjectStoreResponse(const ostk::BaseObjectStoreRequest& request);
    HsmObjectStoreResponse(
        const ostk::BaseObjectStoreRequest& request,
        HsmMiddlewareResponse::Ptr middleware_response);
    HsmObjectStoreResponse(
        const ostk::BaseObjectStoreRequest& request,
        HsmObjectStoreResponse::Ptr hsm_child_response);
    HsmObjectStoreResponse(
        const ostk::BaseObjectStoreRequest& request,
        ostk::ObjectStoreResponse::Ptr child_response);

    static Ptr create(const ostk::BaseObjectStoreRequest& request);
    static Ptr create(
        const ostk::BaseObjectStoreRequest& request,
        HsmMiddlewareResponse::Ptr middlewareResponse);
    static Ptr create(
        const ostk::BaseObjectStoreRequest& request,
        HsmObjectStoreResponse::Ptr hsmChildResponse);
    static Ptr create(
        const ostk::BaseObjectStoreRequest& request,
        ostk::ObjectStoreResponse::Ptr childResponse);

    static ostk::ObjectStoreResponse::Ptr to_base_response(
        const ostk::BaseObjectStoreRequest& request,
        const HsmObjectStoreResponse* response);

  private:
    HsmMiddlewareResponse::Ptr m_middleware_response;
    HsmObjectStoreResponse::Ptr m_hsm_child_response;
    ostk::ObjectStoreResponse::Ptr m_child_response;
};