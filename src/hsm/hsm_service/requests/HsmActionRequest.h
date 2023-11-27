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

    HsmItem::Type get_subject() const;

    const CrudUserContext& get_user_context() const;

    bool is_release_data_action() const;

    bool is_copy_data_action() const;

    bool is_move_data_action() const;

    bool is_put_data_action() const;

    bool is_get_data_action() const;

    bool is_copy_or_move_data_action() const;

    std::string method_as_string() const override;

    std::string subject_as_string() const;

    void set_extent(const Extent& extent);

    uint8_t source_tier() const;

    uint8_t target_tier() const;

    const std::string& source_tier_id() const;

    const std::string& target_tier_id() const;

    std::string to_string() const;

  private:
    CrudUserContext m_user_context;
    HsmAction m_action;
};
}  // namespace hestia
