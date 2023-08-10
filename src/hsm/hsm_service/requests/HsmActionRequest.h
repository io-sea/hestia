#pragma once

#include "CrudRequest.h"
#include "Extent.h"
#include "HsmAction.h"

#include <memory>

namespace hestia {

class HsmActionRequest :
    public BaseRequest,
    public MethodRequest<HsmAction::Action> {
  public:
    using Ptr = std::unique_ptr<HsmActionRequest>;

    HsmActionRequest(
        const HsmAction& action, const CrudUserContext& user_context);

    Extent extent() const;

    const HsmAction& get_action() const;

    const CrudUserContext& get_user_context() const;

    std::string method_as_string() const override;

    std::string subject_as_string() const;

    void set_extent(const Extent& extent);

    uint8_t source_tier() const;

    uint8_t target_tier() const;

    std::string to_string() const;

    HsmItem::Type get_subject() const { return m_action.get_subject(); }

  private:
    CrudUserContext m_user_context;
    HsmAction m_action;
};
}  // namespace hestia
