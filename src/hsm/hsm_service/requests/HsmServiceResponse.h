#pragma once

#include "HsmObjectStoreResponse.h"
#include "HsmServiceError.h"
#include "HsmServiceRequest.h"

#include "HsmObject.h"

#include "Dataset.h"
#include "Response.h"
#include "StorageTier.h"

#include <memory>

namespace hestia {

using ObjectServiceResponse    = CrudResponse<HsmObject, CrudErrorCode>;
using ObjectServiceResponsePtr = std::unique_ptr<ObjectServiceResponse>;

using TierServiceResponse    = CrudResponse<StorageTier, CrudErrorCode>;
using TierServiceResponsePtr = std::unique_ptr<TierServiceResponse>;

using DatasetServiceResponse    = CrudResponse<Dataset, CrudErrorCode>;
using DatasetServiceResponsePtr = std::unique_ptr<DatasetServiceResponse>;

class HsmServiceResponse : public Response<HsmServiceErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmServiceResponse>;

    HsmServiceResponse(const HsmServiceRequest& request);
    HsmServiceResponse(
        const HsmServiceRequest& request, HsmObjectStoreResponse::Ptr response);
    HsmServiceResponse(
        const HsmServiceRequest& request, ObjectServiceResponsePtr response);
    HsmServiceResponse(
        const HsmServiceRequest& request, TierServiceResponsePtr response);
    HsmServiceResponse(
        const HsmServiceRequest& request, DatasetServiceResponsePtr response);

    static Ptr create(const HsmServiceRequest& request);
    static Ptr create(
        const HsmServiceRequest& request, HsmObjectStoreResponse::Ptr response);
    static Ptr create(
        const HsmServiceRequest& request, ObjectServiceResponsePtr response);
    static Ptr create(
        const HsmServiceRequest& request, TierServiceResponsePtr response);
    static Ptr create(
        const HsmServiceRequest& request, DatasetServiceResponsePtr response);

    const std::string& query_result() const;

    const HsmObject& object() const;

    HsmObject& object();

    const Dataset& dataset() const;

    const std::vector<HsmObject>& objects() const;

    const std::vector<StorageTier>& tiers() const;

    const std::vector<Dataset>& datasets() const;

    bool object_found() const;

    void set_object_found(bool found);

    void add_tier(const StorageTier& tier);

    void add_object(const HsmObject& object);

  private:
    std::string m_query_result;

    ObjectServiceResponsePtr m_object_response;
    TierServiceResponsePtr m_tier_response;
    DatasetServiceResponsePtr m_dataset_response;
    HsmObjectStoreResponse::Ptr m_object_store_response;

    std::vector<StorageTier> m_tiers;
    std::vector<HsmObject> m_objects;
};
}  // namespace hestia
