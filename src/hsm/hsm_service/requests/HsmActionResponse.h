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
        const HsmAction& action,
        HsmObjectStoreResponse::Ptr response = nullptr);

    static Ptr create(
        const BaseRequest& request,
        const HsmAction& action,
        HsmObjectStoreResponse::Ptr response = nullptr);

    void set_redirect_location(const std::string& redirect);

    const std::string& get_redirect_location() const;

    HsmAction& action();

    const HsmAction& get_action() const;

  private:
    HsmAction m_action;
    HsmObjectStoreResponse::Ptr m_object_store_response;
    std::string m_redirect_location;
};
}  // namespace hestia
