#pragma once

#include "CrudService.h"

#include "EventFeed.h"

#include "HsmActionResponse.h"
#include "HsmObject.h"
#include "HsmServicesFactory.h"

#include "ErrorUtils.h"
#include "Stream.h"

#include <unordered_map>

namespace hestia {
class HsmObjectStoreClient;
class HsmEventSink;
class KeyValueStoreClient;

class DataPlacementEngine;
class UserService;

class HsmService : public CrudService {
  public:
    using Ptr = std::unique_ptr<HsmService>;

    HsmService(
        const ServiceConfig& config,
        HsmServiceCollection::Ptr service_collection,
        HsmObjectStoreClient* object_store,
        std::unique_ptr<DataPlacementEngine> placement_engine = {},
        EventFeed* event_feed                                 = nullptr);

    static Ptr create(
        const ServiceConfig& config,
        KeyValueStoreClient* client,
        HsmObjectStoreClient* object_store,
        UserService* user_service,
        EventFeed* event_feed);

    virtual ~HsmService();

    CrudService* get_service(HsmItem::Type type) const;

    [[nodiscard]] CrudResponse::Ptr make_request(
        const CrudRequest& request,
        const std::string& type = {}) const noexcept override;

    using actionCompletionFunc = std::function<void(HsmActionResponse::Ptr)>;
    using actionProgressFunc   = std::function<void(HsmActionResponse::Ptr)>;
    void do_hsm_action(
        const HsmActionRequest& request,
        Stream* stream,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func = nullptr) const noexcept;

    void update_tiers(const CrudUserContext& user_id);

  private:
    CrudResponse::Ptr do_crud(
        HsmItem::Type subject_type, const CrudRequest& request) const;

    CrudResponse::Ptr crud_create(
        HsmItem::Type subject_type, const CrudRequest& request) const;

    void get_data(
        const HsmActionRequest& request,
        Stream* stream,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func) const;

    void put_data(
        const HsmActionRequest& request,
        Stream* stream,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func) const;

    void copy_data(
        const HsmActionRequest& request,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func) const;

    void move_data(
        const HsmActionRequest& request,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func) const;

    void release_data(
        const HsmActionRequest& request,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func) const;

    struct ActionContext {
        ActionContext() = default;

        ActionContext(
            const HsmActionRequest& req,
            actionCompletionFunc completion_func,
            actionProgressFunc progress_func) :
            m_action(req.get_action()),
            m_req(req),
            m_user_context(req.get_user_context()),
            m_completion_func(completion_func),
            m_progress_func(progress_func),
            m_source_tier(req.source_tier()),
            m_target_tier(req.target_tier()),
            m_extent(req.extent())
        {
        }

        HsmAction m_action;
        BaseRequest m_req;
        CrudUserContext m_user_context;
        actionCompletionFunc m_completion_func;
        actionProgressFunc m_progress_func;
        uint8_t m_source_tier{0};
        uint8_t m_target_tier{0};
        std::string m_store_id;
        Extent m_extent;
        bool m_needs_db_update{false};
    };

    void make_object_store_request(
        HsmObjectStoreRequestMethod method,
        const ActionContext& action_context,
        Stream* stream = nullptr) const;

    void on_object_store_response(
        HsmObjectStoreRequestMethod method,
        HsmObjectStoreResponse::Ptr object_store_response,
        const ActionContext& action_context) const;

    void on_db_update(
        HsmObjectStoreRequestMethod method,
        const std::string& store_id,
        const ActionContext& action_context) const;

    std::size_t get_object_size_on_tier(
        const ActionContext& action_context) const;

    void init_extent(
        TierExtents& extent,
        const HsmService::ActionContext& action_context,
        const std::string& tier_id,
        const std::string& store_id) const;

    void get_action(
        const ActionContext& action_context, HsmAction& action) const;

    void get_object(
        const ActionContext& action_context, HsmObject& object) const;

    bool get_tier_extent(
        const std::string& tier_id,
        const HsmObject& object,
        TierExtents& extents) const;

    void update_object(
        const ActionContext& action_context, const HsmObject& object) const;

    void update_action(
        const ActionContext& action_context, const HsmAction& action) const;

    CrudResponsePtr get_or_create_action(
        const HsmActionRequest& req, HsmAction& working_action) const;

    void create_or_update_extent(
        const ActionContext& action_context,
        const TierExtents& extent,
        bool do_create) const;

    void remove_or_update_extent(
        const ActionContext& action_context, const TierExtents& extent) const;

    const std::string& get_tier_id(uint8_t tier) const;

    void set_action_error(
        const ActionContext& action_context, const std::string& message) const;

    void set_action_finished_ok(
        const ActionContext& action_context, std::size_t bytes) const;

    void set_action_progress(
        const ActionContext& action_context, std::size_t bytes) const;

    HsmServiceCollection::Ptr m_services;
    HsmObjectStoreClient* m_object_store;
    std::unique_ptr<DataPlacementEngine> m_placement_engine;
    std::unordered_map<uint8_t, std::string> m_tier_cache;
    EventFeed* m_event_feed{nullptr};
};
}  // namespace hestia
