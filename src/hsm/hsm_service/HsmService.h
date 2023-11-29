#pragma once

#include "CrudService.h"

#include "EventFeed.h"

#include "HsmActionContext.h"
#include "HsmObject.h"
#include "HsmServicesFactory.h"

#include "ErrorUtils.h"
#include "Stream.h"

#include <unordered_map>

namespace hestia {
class HsmObjectStoreClient;
class HsmEventSink;
class KeyValueStoreClient;

class UserService;

class HsmService : public CrudService {
  public:
    using Ptr = std::unique_ptr<HsmService>;

    HsmService(
        const ServiceConfig& config,
        HsmServiceCollection::Ptr service_collection,
        HsmObjectStoreClient* object_store,
        EventFeed* event_feed = nullptr);

    static Ptr create(
        const ServiceConfig& config,
        KeyValueStoreClient* client,
        HsmObjectStoreClient* object_store,
        UserService* user_service,
        EventFeed* event_feed);

    virtual ~HsmService();

    using actionCompletionFunc = std::function<void(HsmActionResponse::Ptr)>;
    using actionProgressFunc   = std::function<void(HsmActionResponse::Ptr)>;
    void do_hsm_action(
        const HsmActionRequest& request,
        Stream* stream,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func = nullptr) const noexcept;

    CrudService* get_service(HsmItem::Type type) const;

    const std::string& get_tier_id(uint8_t tier) const;

    [[nodiscard]] CrudResponse::Ptr make_request(
        const CrudRequest& request,
        const std::string& type = {}) const noexcept override;

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

    void make_object_store_request(
        HsmObjectStoreRequestMethod method,
        const HsmActionContext& action_context,
        Stream* stream = nullptr) const;

    void on_object_store_response(
        HsmObjectStoreRequestMethod method,
        HsmObjectStoreResponse::Ptr object_store_response,
        const HsmActionContext& action_context) const;

    void on_db_update(
        HsmObjectStoreRequestMethod method,
        const std::string& store_id,
        const HsmActionContext& action_context,
        std::time_t action_time) const;

    std::size_t get_object_size_on_tier(
        const HsmActionContext& action_context) const;

    void init_extent(
        TierExtents& extent,
        const HsmActionContext& action_context,
        const std::string& tier_id,
        const std::string& store_id) const;

    void get_action(
        const HsmActionContext& action_context, HsmAction& action) const;

    void get_object(
        const HsmActionContext& action_context, HsmObject& object) const;

    bool get_tier_extent(
        const std::string& tier_id,
        const HsmObject& object,
        TierExtents& extents) const;

    void update_object(
        const HsmActionContext& action_context, const HsmObject& object) const;

    void update_action(
        const HsmActionContext& action_context, const HsmAction& action) const;

    CrudResponsePtr get_or_create_action(
        const HsmActionRequest& req, HsmAction& working_action) const;

    void create_or_update_extent(
        const HsmActionContext& action_context,
        const TierExtents& extent,
        bool do_create) const;

    void remove_or_update_extent(
        const HsmActionContext& action_context,
        const TierExtents& extent) const;

    void set_action_error(
        const HsmActionContext& action_context,
        const std::string& message) const;

    void set_action_finished_ok(
        const HsmActionContext& action_context, std::size_t bytes) const;

    void set_action_progress(
        const HsmActionContext& action_context, std::size_t bytes) const;

    void on_metadata_update(
        const std::vector<std::string>& ids, const CrudUserContext& user) const;

    void on_object_create(
        const std::vector<std::string>& ids, const CrudUserContext& user) const;

    void on_object_removed(
        const std::vector<std::string>& ids, const CrudUserContext& user) const;

    HsmServiceCollection::Ptr m_services;
    HsmObjectStoreClient* m_object_store;
    std::unordered_map<uint8_t, std::string> m_tier_cache;
    EventFeed* m_event_feed{nullptr};
};
}  // namespace hestia
