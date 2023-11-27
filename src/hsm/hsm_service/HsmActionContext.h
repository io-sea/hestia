#pragma once

#include "HsmActionResponse.h"

namespace hestia {
using actionCompletionFunc = std::function<void(HsmActionResponse::Ptr)>;
using actionProgressFunc   = std::function<void(HsmActionResponse::Ptr)>;

struct HsmActionContext {
    HsmActionContext() = default;

    HsmActionContext(
        const HsmActionRequest& req,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func) :
        m_action(req.get_action()),
        m_req(req),
        m_user_context(req.get_user_context()),
        m_completion_func(completion_func),
        m_progress_func(progress_func),
        m_source_tier_id(req.source_tier_id()),
        m_target_tier_id(req.target_tier_id()),
        m_source_tier_prio(req.source_tier()),
        m_target_tier_prio(req.target_tier()),
        m_extent(req.extent())
    {
    }

    HsmAction m_action;
    BaseRequest m_req;
    CrudUserContext m_user_context;
    actionCompletionFunc m_completion_func;
    actionProgressFunc m_progress_func;
    std::string m_source_tier_id;
    std::string m_target_tier_id;
    uint8_t m_source_tier_prio;
    uint8_t m_target_tier_prio;
    std::string m_store_id;
    Extent m_extent;
    bool m_needs_db_update{false};
};
}  // namespace hestia