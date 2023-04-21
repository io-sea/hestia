#pragma once

#include "HsmObjectStoreResponse.h"

#include "HsmServiceError.h"
#include "HsmServiceRequest.h"

#include <ostk/Response.h>

#include <memory>

class HsmServiceResponse : public ostk::Response<HsmServiceErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmServiceResponse>;

    HsmServiceResponse(const HsmServiceRequest& request);
    HsmServiceResponse(
        const HsmServiceRequest& request,
        HsmObjectStoreResponse::Ptr object_store_response);
    HsmServiceResponse(
        const HsmServiceRequest& request,
        ostk::ObjectStoreResponse::Ptr kv_store_response);

    static Ptr create(const HsmServiceRequest& request);
    static Ptr create(
        const HsmServiceRequest& request,
        HsmObjectStoreResponse::Ptr objectStoreResponse);
    static Ptr create(
        const HsmServiceRequest& request,
        ostk::ObjectStoreResponse::Ptr kvStoreResponse);

    const std::string& query_result() const;

    const std::vector<ostk::Uuid>& objects() const;

    const std::vector<uint8_t>& tiers() const;

  private:
    std::string m_query_result;
    std::vector<ostk::Uuid> m_object_ids;
    std::vector<uint8_t> m_tiers;

    HsmObjectStoreResponse::Ptr m_object_store_response;
    ostk::ObjectStoreResponse::Ptr m_key_value_store_response;
};