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

    void set_redirect_location(const std::string& redirect)
    {
        m_redirect_location = redirect;
    }

    const std::string& get_redirect_location() const
    {
        return m_redirect_location;
    }

  private:
    HsmObjectStoreResponse::Ptr m_object_store_response;
    std::string m_redirect_location;
};
}  // namespace hestia
