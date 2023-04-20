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
        HsmMiddlewareResponse::Ptr middlewareResponse);
    HsmObjectStoreResponse(
        const ostk::BaseObjectStoreRequest& request,
        HsmObjectStoreResponse::Ptr hsmChildResponse);
    HsmObjectStoreResponse(
        const ostk::BaseObjectStoreRequest& request,
        ostk::ObjectStoreResponse::Ptr childResponse);

    static Ptr Create(const ostk::BaseObjectStoreRequest& request);
    static Ptr Create(
        const ostk::BaseObjectStoreRequest& request,
        HsmMiddlewareResponse::Ptr middlewareResponse);
    static Ptr Create(
        const ostk::BaseObjectStoreRequest& request,
        HsmObjectStoreResponse::Ptr hsmChildResponse);
    static Ptr Create(
        const ostk::BaseObjectStoreRequest& request,
        ostk::ObjectStoreResponse::Ptr childResponse);

    static ostk::ObjectStoreResponse::Ptr toBaseResponse(
        const ostk::BaseObjectStoreRequest& request,
        const HsmObjectStoreResponse* response);

  private:
    HsmMiddlewareResponse::Ptr mMiddlewareResponse;
    HsmObjectStoreResponse::Ptr mHsmChildResponse;
    ostk::ObjectStoreResponse::Ptr mChildResponse;
};