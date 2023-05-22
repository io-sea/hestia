#pragma once

#include "HsmObjectStoreResponse.h"

#include "HsmServiceError.h"
#include "HsmServiceRequest.h"

#include "Response.h"

#include <memory>

namespace hestia {
class HsmServiceResponse : public hestia::Response<HsmServiceErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmServiceResponse>;

    HsmServiceResponse(const HsmServiceRequest& request);
    HsmServiceResponse(
        const HsmServiceRequest& request,
        HsmObjectStoreResponse::Ptr object_store_response);
    HsmServiceResponse(
        const HsmServiceRequest& request,
        hestia::ObjectStoreResponse::Ptr kv_store_response);

    static Ptr create(const HsmServiceRequest& request);
    static Ptr create(
        const HsmServiceRequest& request,
        HsmObjectStoreResponse::Ptr object_store_response);
    static Ptr create(
        const HsmServiceRequest& request,
        hestia::ObjectStoreResponse::Ptr kv_store_response);

    const std::string& query_result() const;

    const std::vector<hestia::Uuid>& objects() const;

    const std::vector<uint8_t>& tiers() const;

  private:
    std::string m_query_result;
    std::vector<hestia::Uuid> m_object_ids;
    std::vector<uint8_t> m_tiers;

    HsmObjectStoreResponse::Ptr m_object_store_response;
    hestia::ObjectStoreResponse::Ptr m_key_value_store_response;
};
}  // namespace hestia
