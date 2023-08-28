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
class KeyValueStoreClient;

class DataPlacementEngine;
class UserService;
class EventFeed;

class HsmService : public CrudService {
  public:
    using Ptr = std::unique_ptr<HsmService>;

    HsmService(
        const ServiceConfig& config,
        HsmServiceCollection::Ptr service_collection,
        HsmObjectStoreClient* object_store,
        std::unique_ptr<DataPlacementEngine> placement_engine = {});

    static Ptr create(
        const ServiceConfig& config,
        KeyValueStoreClient* client,
        HsmObjectStoreClient* object_store,
        UserService* user_service,
        EventFeed* event_feed);

    virtual ~HsmService();

    CrudService* get_service(HsmItem::Type type);

    [[nodiscard]] CrudResponse::Ptr make_request(
        const CrudRequest& request,
        const std::string& type = {}) const noexcept override;

    [[nodiscard]] HsmActionResponse::Ptr make_request(
        const HsmActionRequest& request) const noexcept;

    using dataIoCompletionFunc = std::function<void(HsmActionResponse::Ptr)>;
    void do_data_io_action(
        const HsmActionRequest& request,
        Stream* stream,
        dataIoCompletionFunc completion_func) const;

    void update_tiers(const std::string& user_id);

  private:
    CrudResponse::Ptr crud_create(
        HsmItem::Type subject_type, const CrudRequest& request) const noexcept;
    CrudResponse::Ptr crud_read(
        HsmItem::Type subject_type, const CrudRequest& request) const noexcept;
    CrudResponse::Ptr crud_update(
        HsmItem::Type subject_type, const CrudRequest& request) const noexcept;
    CrudResponse::Ptr crud_remove(
        HsmItem::Type subject_type, const CrudRequest& request) const noexcept;
    CrudResponse::Ptr crud_identify(
        HsmItem::Type subject_type, const CrudRequest& request) const noexcept;

    void get_data(
        const HsmActionRequest& request,
        Stream* stream,
        dataIoCompletionFunc completion_func) const noexcept;
    void put_data(
        const HsmActionRequest& request,
        Stream* stream,
        dataIoCompletionFunc completion_func) const noexcept;
    HsmActionResponse::Ptr copy_data(
        const HsmActionRequest& request) const noexcept;
    HsmActionResponse::Ptr move_data(
        const HsmActionRequest& request) const noexcept;
    HsmActionResponse::Ptr release_data(
        const HsmActionRequest& request) const noexcept;

    void on_put_data_complete(
        const BaseRequest& req,
        const CrudUserContext& user_context,
        const HsmObject& working_object,
        uint8_t tier,
        const Extent& extent,
        const std::string& store_id,
        const HsmAction& working_action,
        dataIoCompletionFunc completion_func) const;

    void on_get_data_complete(
        const BaseRequest& req,
        const HsmAction& working_action,
        dataIoCompletionFunc completion_func) const;

    CrudResponsePtr get_or_create_action(
        const HsmActionRequest& req, HsmAction& working_action) const;

    const std::string& get_tier_id(uint8_t tier) const;

    void set_action_error(
        const CrudUserContext& user_context,
        const std::string& action_id,
        const std::string& message);

    void set_action_finished_ok(const std::string& action_id);

    void set_action_progress(const std::string& action_id, std::size_t bytes);

    HsmServiceCollection::Ptr m_services;
    HsmObjectStoreClient* m_object_store;
    std::unique_ptr<DataPlacementEngine> m_placement_engine;
    std::unordered_map<uint8_t, std::string> m_tier_cache;
};
}  // namespace hestia
