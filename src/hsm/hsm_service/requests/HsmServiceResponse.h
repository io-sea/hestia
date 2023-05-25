#pragma once

#include "HsmObjectStoreResponse.h"
#include "HsmServiceError.h"
#include "HsmServiceRequest.h"

#include "HsmObject.h"

#include "Response.h"
#include "StorageTier.h"

#include <memory>

namespace hestia {

using ObjectServiceResponse    = CrudResponse<HsmObject, CrudErrorCode>;
using ObjectServiceResponsePtr = std::unique_ptr<ObjectServiceResponse>;

using TierServiceResponse    = CrudResponse<StorageTier, CrudErrorCode>;
using TierServiceResponsePtr = std::unique_ptr<TierServiceResponse>;

class HsmServiceResponse : public Response<HsmServiceErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmServiceResponse>;

    HsmServiceResponse(const HsmServiceRequest& request);
    HsmServiceResponse(
        const HsmServiceRequest& request,
        HsmObjectStoreResponse::Ptr object_store_response);
    HsmServiceResponse(
        const HsmServiceRequest& request,
        ObjectServiceResponsePtr object_service_response);
    HsmServiceResponse(
        const HsmServiceRequest& request,
        TierServiceResponsePtr tier_service_response);

    static Ptr create(const HsmServiceRequest& request);
    static Ptr create(
        const HsmServiceRequest& request,
        HsmObjectStoreResponse::Ptr object_store_response);
    static Ptr create(
        const HsmServiceRequest& request,
        ObjectServiceResponsePtr object_service_response);
    static Ptr create(
        const HsmServiceRequest& request,
        TierServiceResponsePtr tier_service_response);

    const std::string& query_result() const;

    const HsmObject& object() const;

    HsmObject& object();

    const std::vector<HsmObject>& objects() const;

    const std::vector<StorageTier>& tiers() const;

    bool object_found() const;

    void set_object_found(bool found);

  private:
    std::string m_query_result;
    ObjectServiceResponsePtr m_object_response;
    TierServiceResponsePtr m_tier_response;
    HsmObjectStoreResponse::Ptr m_object_store_response;
};
}  // namespace hestia
