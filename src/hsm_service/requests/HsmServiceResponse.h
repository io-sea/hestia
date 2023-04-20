#pragma once

#include "HsmObjectStoreResponse.h"

#include "HsmServiceRequest.h"
#include "HsmServiceError.h"

#include <ostk/Response.h>

#include <memory>

class HsmServiceResponse : public ostk::Response<HsmServiceErrorCode>
{
public:
    using Ptr = std::unique_ptr<HsmServiceResponse>;

    HsmServiceResponse(const HsmServiceRequest& request);
    HsmServiceResponse(const HsmServiceRequest& request, HsmObjectStoreResponse::Ptr objectStoreResponse);
    HsmServiceResponse(const HsmServiceRequest& request, ostk::ObjectStoreResponse::Ptr kvStoreResponse);

    static Ptr Create(const HsmServiceRequest& request);
    static Ptr Create(const HsmServiceRequest& request, HsmObjectStoreResponse::Ptr objectStoreResponse);
    static Ptr Create(const HsmServiceRequest& request, ostk::ObjectStoreResponse::Ptr kvStoreResponse);

    const std::string& queryResult() const;

    const std::vector<ostk::Uuid>& objects() const;

    const std::vector<uint8_t>& tiers() const;

private:
    std::string mQueryResult;
    std::vector<ostk::Uuid> mObjectIds;
    std::vector<uint8_t> mTiers;

    HsmObjectStoreResponse::Ptr mObjectStoreResponse;
    ostk::ObjectStoreResponse::Ptr mKeyValueStoreResponse;
};