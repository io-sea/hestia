#pragma once

#include "Extent.h"
#include "HsmAction.h"
#include "Request.h"

#include <memory>

namespace hestia {

class HsmActionRequest :
    public BaseRequest,
    public MethodRequest<HsmAction::Action> {
  public:
    using Ptr = std::unique_ptr<HsmActionRequest>;

    HsmActionRequest(const HsmAction& action, const std::string& user_id);

    Extent extent() const;

    const HsmAction& get_action() const;

    const std::string& get_user_id() const;

    std::string method_as_string() const override;

    std::string subject_as_string() const;

    void set_extent(const Extent& extent);

    uint8_t source_tier() const;

    uint8_t target_tier() const;

    std::string to_string() const;

    HsmItem::Type get_subject() const { return m_action.get_subject(); }

  private:
    std::string m_user_id;
    HsmAction m_action;
};
}  // namespace hestia
