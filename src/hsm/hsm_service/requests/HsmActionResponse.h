#pragma once

#include "HsmActionError.h"
#include "HsmActionRequest.h"
#include "HsmObjectStoreResponse.h"

namespace hestia {

class HsmActionResponse : public Response<HsmActionErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmActionResponse>;
    HsmActionResponse(
        const BaseRequest& request,
        HsmObjectStoreResponse::Ptr response = nullptr);

    static Ptr create(
        const BaseRequest& request,
        HsmObjectStoreResponse::Ptr response = nullptr);

  private:
    HsmObjectStoreResponse::Ptr m_object_store_response;
};
}  // namespace hestia
