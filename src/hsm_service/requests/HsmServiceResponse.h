#pragma once

#include "HsmObjectStoreResponse.h"

#include "HsmServiceError.h"
#include "HsmServiceRequest.h"

#include "Response.h"

#include <memory>

namespace hestia {
class HsmServiceResponse : public Response<HsmServiceErrorCode> {
  public:
    using Ptr = std::unique_ptr<HsmServiceResponse>;

    HsmServiceResponse(const HsmServiceRequest& request);
    HsmServiceResponse(
        const HsmServiceRequest& request, BaseResponse::Ptr child_response);

    static Ptr create(const HsmServiceRequest& request);
    static Ptr create(
        const HsmServiceRequest& request, BaseResponse::Ptr child_response);

    const std::string& query_result() const;

    const std::vector<Uuid>& objects() const;

    const std::vector<uint8_t>& tiers() const;

    bool object_found() const { return m_object_found; }

    void set_object_found(bool found) { m_object_found = found; }

  private:
    std::string m_query_result;
    std::vector<Uuid> m_object_ids;
    std::vector<uint8_t> m_tiers;
    bool m_object_found{false};

    BaseResponse::Ptr m_child_response;
};
}  // namespace hestia
