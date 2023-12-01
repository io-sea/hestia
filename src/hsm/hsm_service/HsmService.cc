#include "HsmService.h"

#include "HsmObjectStoreClient.h"

#include "CrudClient.h"
#include "CrudService.h"
#include "CrudServiceBackend.h"
#include "TypedCrudRequest.h"

#include "IdGenerator.h"
#include "StorageTier.h"
#include "TimeProvider.h"

#include "ErrorUtils.h"
#include "TimeUtils.h"
#include "UuidUtils.h"

#include "HsmAction.h"
#include "HsmObject.h"

#include "KeyValueStoreClient.h"
#include "Logger.h"

#include <cassert>
#include <set>

#include <iostream>

#define CRUD_ERROR_CHECK(response, action_context)                             \
    if (!response->ok()) {                                                     \
        const std::string msg = response->get_error().to_string();             \
        throw RequestException<HsmActionError>(                                \
            {HsmActionErrorCode::CRUD_ERROR, msg});                            \
    }

#define CATCH_FLOW()                                                           \
    catch (const RequestException<HsmActionError>& e)                          \
    {                                                                          \
        const std::string msg = SOURCE_LOC() + " | Exception: " + e.what();    \
        LOG_ERROR(msg);                                                        \
        response->on_error(e.get_error());                                     \
        completion_func(std::move(response));                                  \
    }                                                                          \
    catch (const std::exception& e)                                            \
    {                                                                          \
        const std::string msg = SOURCE_LOC() + " | Exception: " + e.what();    \
        LOG_ERROR(msg);                                                        \
        response->on_error({HsmActionErrorCode::STL_EXCEPTION, msg});          \
        completion_func(std::move(response));                                  \
    }                                                                          \
    catch (...)                                                                \
    {                                                                          \
        const std::string msg = SOURCE_LOC() + " | Unknown Exception.";        \
        LOG_ERROR(msg);                                                        \
        response->on_error({HsmActionErrorCode::STL_EXCEPTION, msg});          \
        completion_func(std::move(response));                                  \
    }

namespace hestia {

HsmService::HsmService(
    const ServiceConfig& config,
    HsmServiceCollection::Ptr service_collection,
    HsmObjectStoreClient* object_store,
    EventFeed* event_feed) :
    CrudService(config),
    m_services(std::move(service_collection)),
    m_object_store(object_store),
    m_event_feed(event_feed)
{
    LOG_INFO("Creating HsmService");
}

HsmService::Ptr HsmService::create(
    const ServiceConfig& config,
    KeyValueStoreClient* client,
    HsmObjectStoreClient* object_store,
    UserService* user_service,
    EventFeed* event_feed)
{
    KeyValueStoreCrudServiceBackend backend(client);

    auto services = std::make_unique<HsmServiceCollection>();
    services->create_default_services(config, &backend, user_service);

    return std::make_unique<HsmService>(
        config, std::move(services), object_store, event_feed);
}

HsmService::~HsmService() {}

CrudService* HsmService::get_service(HsmItem::Type type) const
{
    return m_services->get_service(type);
}

CrudResponse::Ptr HsmService::make_request(
    const CrudRequest& req, const std::string& type) const noexcept
{
    LOG_INFO("Request " << type << " " << req.method_as_string());

    auto response = CrudResponse::create(req, type, req.get_output_format());
    switch (req.method()) {
        case CrudMethod::CREATE:
            try {
                const auto hsm_type = HsmItem::from_name(type);
                response            = crud_create(hsm_type, req);
                if (m_event_feed != nullptr
                    && hsm_type == HsmItem::Type::OBJECT) {
                    std::vector<std::string> ids;
                    for (const auto& id : response->get_ids().data()) {
                        ids.push_back(id.get_primary_key());
                    }
                    on_object_create(ids, req.get_user_context());
                }
            }
            catch (const std::exception& e) {
                const std::string msg =
                    SOURCE_LOC() + " | Exception in CRUD: " + e.what();
                LOG_ERROR(msg);
                response->on_error({CrudErrorCode::STL_EXCEPTION, msg});
            }
            return response;
        case CrudMethod::UPDATE:
            try {
                const auto hsm_type = HsmItem::from_name(type);
                response            = do_crud(hsm_type, req);
                if (m_event_feed != nullptr) {
                    std::set<std::string> ids;
                    for (const auto& id : response->get_ids().data()) {
                        ids.insert(id.get_primary_key());
                    }
                    if (hsm_type == HsmItem::Type::METADATA) {
                        on_metadata_update(
                            {ids.begin(), ids.end()}, req.get_user_context());
                    }
                    else if (hsm_type == HsmItem::Type::ACTION) {
                        on_action_update(
                            {ids.begin(), ids.end()}, req.get_user_context());
                    }
                }
            }
            catch (const std::exception& e) {
                const std::string msg =
                    SOURCE_LOC() + " | Exception in CRUD: " + e.what();
                LOG_ERROR(msg);
                response->on_error({CrudErrorCode::STL_EXCEPTION, msg});
            }
            return response;
        case CrudMethod::REMOVE:
            try {
                const auto hsm_type = HsmItem::from_name(type);
                response            = do_crud(hsm_type, req);
                if (m_event_feed != nullptr
                    && hsm_type == HsmItem::Type::OBJECT) {
                    std::vector<std::string> ids;
                    for (const auto& id : response->get_ids().data()) {
                        ids.push_back(id.get_primary_key());
                    }
                    on_object_removed(ids, req.get_user_context());
                }
            }
            catch (const std::exception& e) {
                const std::string msg =
                    SOURCE_LOC() + " | Exception in CRUD: " + e.what();
                LOG_ERROR(msg);
                response->on_error({CrudErrorCode::STL_EXCEPTION, msg});
            }
            return response;
        case CrudMethod::READ:
        case CrudMethod::IDENTIFY:
            try {
                response = do_crud(HsmItem::from_name(type), req);
            }
            catch (const std::exception& e) {
                const std::string msg =
                    SOURCE_LOC() + " | Exception in CRUD: " + e.what();
                LOG_ERROR(msg);
                response->on_error({CrudErrorCode::STL_EXCEPTION, msg});
            }
            return response;
        case CrudMethod::LOCK:
        case CrudMethod::UNLOCK:
        case CrudMethod::LOCKED:
        default:
            response->on_error(
                {CrudErrorCode::UNSUPPORTED_REQUEST_METHOD,
                 "Locking not supported yet."});
            return response;
    }
}

void HsmService::on_object_removed(
    const std::vector<std::string>& ids, const CrudUserContext&) const
{
    for (const auto& id : ids) {
        CrudEvent event(HsmItem::hsm_object_name, CrudMethod::REMOVE, {id});
        m_event_feed->on_event(event);
    }
}

void HsmService::on_object_create(
    const std::vector<std::string>& ids, const CrudUserContext& user) const
{
    for (const auto& id : ids) {
        const auto object_service =
            m_services->get_service(HsmItem::Type::OBJECT);
        const auto get_response = object_service->make_request(CrudRequest{
            CrudMethod::READ, CrudQuery(id, CrudQuery::BodyFormat::ITEM),
            user});
        if (!get_response->ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to read object for event feed");
        }
        if (!get_response->found()) {
            throw RequestException<HsmActionError>(
                {HsmActionErrorCode::ITEM_NOT_FOUND,
                 SOURCE_LOC() + " | Object " + id + " not found."});
        }
        auto object = *get_response->get_item_as<HsmObject>();
        object.set_content_modified_time(object.get_last_modified_time());
        object.set_content_accessed_time(object.get_last_modified_time());

        auto object_put_response = object_service->make_request(
            TypedCrudRequest<HsmObject>{CrudMethod::UPDATE, object, user});
        if (!object_put_response->ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to update object for event feed");
        }
        CrudEvent event(
            HsmItem::hsm_object_name, CrudMethod::CREATE,
            {object.get_primary_key()}, object.get_creation_time());
        m_event_feed->on_event(event);
    }
}

void HsmService::on_action_update(
    const std::vector<std::string>& ids, const CrudUserContext& user) const
{
    // This method is called if the object store was updated on a remote - the
    // event is finished and now we update the event feed and object
    // modification times here.
    auto action_service = get_service(HsmItem::Type::ACTION);
    for (const auto& id : ids) {
        auto response = action_service->make_request(CrudRequest(
            CrudMethod::READ, {id, CrudQuery::BodyFormat::ITEM}, user));
        if (!response->ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to read action for event feed");
        }
        if (!response->found()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to find action for event feed");
        }

        auto action = response->get_item_as<HsmAction>();
        if (action->get_status() != HsmAction::Status::FINISHED_OK) {
            continue;
        }

        LOG_INFO("Updating event feed from remote for action id: " << id);
        const auto action_type = action->get_action();

        const auto object_id = action->get_subject_key();
        const auto object_service =
            m_services->get_service(HsmItem::Type::OBJECT);
        const auto get_response = object_service->make_request(CrudRequest{
            CrudMethod::READ, CrudQuery(object_id, CrudQuery::BodyFormat::ITEM),
            user});
        if (!get_response->ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to read object for event feed");
        }
        if (!get_response->found()) {
            throw RequestException<HsmActionError>(
                {HsmActionErrorCode::ITEM_NOT_FOUND,
                 SOURCE_LOC() + " | Object " + object_id + " not found."});
        }
        auto object = *get_response->get_item_as<HsmObject>();

        const auto event_time = TimeUtils::get_current_time();
        if (action_type == HsmAction::Action::GET_DATA) {
            object.set_content_accessed_time(event_time);
            CrudEvent event(
                HsmItem::hsm_object_name, CrudMethod::READ, {object_id},
                event_time);
            m_event_feed->on_event(event);
        }
        else if (
            action_type == HsmAction::Action::PUT_DATA
            || action_type == HsmAction::Action::COPY_DATA
            || action_type == HsmAction::Action::MOVE_DATA
            || action_type == HsmAction::Action::RELEASE_DATA) {
            object.set_content_accessed_time(event_time);
            object.set_content_modified_time(event_time);
            CrudEvent event(
                HsmItem::hsm_object_name, CrudMethod::UPDATE, {object_id},
                event_time);
            m_event_feed->on_event(event);
        }

        auto object_put_response = object_service->make_request(
            TypedCrudRequest<HsmObject>{CrudMethod::UPDATE, object, user});
        if (!object_put_response->ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to update object for event feed");
        }
    }
}

void HsmService::on_metadata_update(
    const std::vector<std::string>& ids, const CrudUserContext& user) const
{
    auto md_service = get_service(HsmItem::Type::METADATA);
    for (const auto& id : ids) {
        auto response = md_service->make_request(CrudRequest(
            CrudMethod::READ, {id, CrudQuery::BodyFormat::ITEM}, user));
        if (!response->ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to read metadata for event feed");
        }
        if (!response->found()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to find metadata for event feed");
        }
        const auto object_id = response->get_item_as<UserMetadata>()->object();

        const auto object_service =
            m_services->get_service(HsmItem::Type::OBJECT);
        const auto get_response = object_service->make_request(CrudRequest{
            CrudMethod::READ, CrudQuery(object_id, CrudQuery::BodyFormat::ITEM),
            user});
        if (!get_response->ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to read object for event feed");
        }
        if (!get_response->found()) {
            throw RequestException<HsmActionError>(
                {HsmActionErrorCode::ITEM_NOT_FOUND,
                 SOURCE_LOC() + " | Object " + object_id + " not found."});
        }
        auto object = *get_response->get_item_as<HsmObject>();

        const auto modified_time = TimeUtils::get_current_time();
        object.set_content_modified_time(modified_time);

        auto object_put_response = object_service->make_request(
            TypedCrudRequest<HsmObject>{CrudMethod::UPDATE, object, user});
        if (!object_put_response->ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + "Failed to update object for event feed");
        }
        CrudEvent event(
            HsmItem::hsm_object_name, CrudMethod::UPDATE,
            {object.get_primary_key()}, modified_time);
        m_event_feed->on_event(event);
    }
}

CrudResponse::Ptr HsmService::crud_create(
    HsmItem::Type subject_type, const CrudRequest& req) const
{
    LOG_INFO("Starting HSMService CREATE");
    CrudService* service{nullptr};
    try {
        service = m_services->get_service(subject_type);
    }
    catch (const std::exception& e) {
        auto response = CrudResponse::create(
            req, HsmItem::to_name(subject_type), CrudQuery::BodyFormat::NONE);
        response->on_error(
            {CrudErrorCode::ERROR, SOURCE_LOC() + " | " + e.what()});
        return response;
    }
    auto response = service->make_request(req);

    LOG_INFO("Finished HSMService CREATE");
    return response;
}

CrudResponse::Ptr HsmService::do_crud(
    HsmItem::Type subject_type, const CrudRequest& req) const
{
    auto response = m_services->get_service(subject_type)->make_request(req);
    return response;
}

void HsmService::do_hsm_action(
    const HsmActionRequest& req,
    Stream* stream,
    actionCompletionFunc completion_func,
    actionProgressFunc progress_func) const noexcept
{
    auto response = HsmActionResponse::create(req, req.get_action());

    switch (req.method()) {
        case HsmAction::Action::COPY_DATA:
            try {
                copy_data(req, completion_func, progress_func);
            }
            CATCH_FLOW()
            return;
        case HsmAction::Action::MOVE_DATA:
            try {
                move_data(req, completion_func, progress_func);
            }
            CATCH_FLOW()
            return;
        case HsmAction::Action::RELEASE_DATA:
            try {
                release_data(req, completion_func, progress_func);
            }
            CATCH_FLOW()
            return;
        case HsmAction::Action::PUT_DATA:
            try {
                put_data(req, stream, completion_func, progress_func);
            }
            CATCH_FLOW()
            return;
        case HsmAction::Action::GET_DATA:
            try {
                get_data(req, stream, completion_func, progress_func);
            }
            CATCH_FLOW()
            return;
        case HsmAction::Action::CRUD:
        case HsmAction::Action::NONE:
        default:
            response->on_error(
                {HsmActionErrorCode::UNSUPPORTED_REQUEST_METHOD,
                 "Unsupported request method."});
            completion_func(std::move(response));
            return;
    }
}

void HsmService::get_object(
    const HsmActionContext& action_context, HsmObject& object) const
{
    const auto object_id = action_context.m_action.get_subject_key();

    const auto object_service = m_services->get_service(HsmItem::Type::OBJECT);
    const auto get_response   = object_service->make_request(CrudRequest{
        CrudMethod::READ, CrudQuery(object_id, CrudQuery::BodyFormat::ITEM),
        action_context.m_user_context});
    CRUD_ERROR_CHECK(get_response, action_context);

    if (!get_response->found()) {
        throw RequestException<HsmActionError>(
            {HsmActionErrorCode::ITEM_NOT_FOUND,
             SOURCE_LOC() + " | Object " + object_id + " not found."});
    }
    object = *get_response->get_item_as<HsmObject>();
}

void HsmService::update_tiers(const CrudUserContext& user_id)
{
    auto tier_service = m_services->get_service(HsmItem::Type::TIER);
    auto get_response = tier_service->make_request(CrudRequest(
        CrudMethod::READ, CrudQuery{CrudQuery::BodyFormat::ITEM}, user_id));
    if (!get_response->ok()) {
        throw RequestException<HsmActionError>(
            {HsmActionErrorCode::CRUD_ERROR,
             SOURCE_LOC() + "Failed listing tiers in cache request"});
    }
    for (const auto& item : get_response->items()) {
        auto tier_item = dynamic_cast<StorageTier*>(item.get());
        m_tier_cache[tier_item->get_priority()] = tier_item->get_primary_key();
    }
}

CrudResponsePtr HsmService::get_or_create_action(
    const HsmActionRequest& req, HsmAction& working_action) const
{
    if (working_action.get_primary_key().empty()) {
        auto action_service = m_services->get_service(HsmItem::Type::ACTION);
        auto action_response =
            action_service->make_request(TypedCrudRequest<HsmAction>{
                CrudMethod::CREATE, working_action, CrudQuery::BodyFormat::ITEM,
                req.get_user_context()});
        if (!action_response->ok()) {
            throw RequestException<HsmActionError>(
                {HsmActionErrorCode::CRUD_ERROR,
                 SOURCE_LOC() + "Failed to create action"});
        }
        working_action = *action_response->get_item_as<HsmAction>();
    }
    return CrudResponse::create(
        req, HsmItem::hsm_action_name, CrudQuery::BodyFormat::NONE);
}

void HsmService::make_object_store_request(
    HsmObjectStoreRequestMethod method,
    const HsmActionContext& action_context,
    Stream* stream) const
{
    const auto object_id = action_context.m_action.get_subject_key();
    const auto action_id = action_context.m_action.get_primary_key();

    LOG_INFO(
        "Starting HSMService "
        << HsmObjectStoreRequest::to_string(method) << ": "
        << " | Subject Id: " << object_id << " | Action Id: " << action_id);

    StorageObject storage_object(object_id);
    storage_object.set_metadata(
        "hestia-user_token", action_context.m_user_context.m_token);

    HsmObjectStoreRequest req(storage_object, method);
    req.set_target_tier(action_context.m_target_tier_id);
    req.set_source_tier(action_context.m_source_tier_id);
    req.set_action_id(action_context.m_action.get_primary_key());
    req.set_extent(action_context.m_extent);

    auto object_store_completion_func =
        [this, method,
         action_context](HsmObjectStoreResponse::Ptr object_store_response) {
            this->on_object_store_response(
                method, std::move(object_store_response), action_context);
        };
    if (action_context.m_progress_func == nullptr) {
        m_object_store->make_request(
            req, object_store_completion_func, nullptr, stream);
    }
    else {
        auto object_store_progress_func =
            [req, action_context](
                HsmObjectStoreResponse::Ptr object_store_response) {
                auto action_context_cp = action_context;
                action_context_cp.m_action.set_num_transferred(
                    object_store_response->get_bytes_transferred());
                action_context_cp.m_progress_func(
                    HsmActionResponse::create(req, action_context_cp.m_action));
            };

        m_object_store->make_request(
            req, object_store_completion_func, object_store_progress_func,
            stream);
    }
}

void HsmService::on_object_store_response(
    HsmObjectStoreRequestMethod method,
    HsmObjectStoreResponse::Ptr object_store_response,
    const HsmActionContext& action_context) const
{
    LOG_INFO("Got object store response");
    if (!object_store_response->ok()) {
        auto response = HsmActionResponse::create(
            action_context.m_req, action_context.m_action);
        const std::string msg =
            SOURCE_LOC() + " | Object store error.\n"
            + object_store_response->get_error().to_string();
        response->on_error({HsmActionErrorCode::OBJECT_STORE_ERROR, msg});
        action_context.m_completion_func(std::move(response));
        return;
    }

    if (!object_store_response->object_is_remote()) {
        const auto action_time = TimeUtils::get_current_time();

        on_db_update(
            method, object_store_response->get_store_id(), action_context,
            action_time);

        if (m_event_feed != nullptr) {
            LOG_INFO("Updating event feed locally");
            const auto object_id = action_context.m_action.get_subject_key();
            if (method == HsmObjectStoreRequestMethod::GET) {
                CrudEvent event(
                    HsmItem::hsm_object_name, CrudMethod::READ, {object_id},
                    action_time);
                m_event_feed->on_event(event);
            }
            else if (
                method == HsmObjectStoreRequestMethod::PUT
                || method == HsmObjectStoreRequestMethod::COPY
                || method == HsmObjectStoreRequestMethod::MOVE
                || method == HsmObjectStoreRequestMethod::REMOVE
                || method == HsmObjectStoreRequestMethod::REMOVE_ALL) {
                CrudEvent event(
                    HsmItem::hsm_object_name, CrudMethod::UPDATE, {object_id},
                    action_time);
                m_event_feed->on_event(event);
            }
        }
    }

    // respond ok
    LOG_INFO("Finished HSM Action ok");
    auto response = HsmActionResponse::create(
        action_context.m_req, action_context.m_action);
    action_context.m_completion_func(std::move(response));
}

void HsmService::on_db_update(
    HsmObjectStoreRequestMethod method,
    const std::string& store_id,
    const HsmActionContext& action_context,
    std::time_t action_time) const
{
    LOG_INFO("Starting DB Update");

    HsmObject working_object;
    get_object(action_context, working_object);

    if (method == HsmObjectStoreRequestMethod::PUT
        || method == HsmObjectStoreRequestMethod::COPY
        || method == HsmObjectStoreRequestMethod::MOVE) {

        TierExtents target_extent;
        bool extent_found = get_tier_extent(
            action_context.m_target_tier_id, working_object, target_extent);
        if (!extent_found) {
            init_extent(
                target_extent, action_context, action_context.m_target_tier_id,
                store_id);
        }
        target_extent.add_extent(action_context.m_extent);
        create_or_update_extent(action_context, target_extent, !extent_found);

        if (target_extent.get_size() > working_object.size()) {
            working_object.set_size(target_extent.get_size());
        }

        if (method == HsmObjectStoreRequestMethod::MOVE) {
            TierExtents source_extent;
            get_tier_extent(
                action_context.m_source_tier_id, working_object, source_extent);
            source_extent.remove_extent(action_context.m_extent);
            remove_or_update_extent(action_context, source_extent);
        }
        working_object.set_content_modified_time(action_time);
        working_object.set_content_accessed_time(action_time);
    }
    else if (method == HsmObjectStoreRequestMethod::REMOVE) {
        TierExtents source_extent;
        get_tier_extent(
            action_context.m_source_tier_id, working_object, source_extent);
        source_extent.remove_extent(action_context.m_extent);
        remove_or_update_extent(action_context, source_extent);
        working_object.set_content_modified_time(action_time);
        working_object.set_content_accessed_time(action_time);
    }
    else if (method == HsmObjectStoreRequestMethod::GET) {
        working_object.set_content_accessed_time(action_time);
    }

    // Update the object
    update_object(action_context, working_object);

    // Report the action complete
    set_action_finished_ok(action_context, action_context.m_extent.m_length);
}

void HsmService::init_extent(
    TierExtents& extent,
    const HsmActionContext& action_context,
    const std::string& tier_id,
    const std::string& store_id) const
{
    extent.set_object_id(action_context.m_action.get_subject_key());
    extent.set_tier_id(tier_id);
    extent.set_tier_priority(action_context.m_target_tier_prio);
    extent.set_backend_id(store_id);
}

bool HsmService::get_tier_extent(
    const std::string& tier_id,
    const HsmObject& object,
    TierExtents& extents) const
{
    for (const auto& tier_extent : object.tiers()) {
        if (tier_extent.get_tier_id() == tier_id) {
            extents = tier_extent;
            return true;
        }
    }
    return false;
}

void HsmService::remove_or_update_extent(
    const HsmActionContext& action_context, const TierExtents& extent) const
{
    CrudResponsePtr extent_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent.empty()) {
        extent_response = extent_service->make_request(CrudRequest(
            CrudMethod::REMOVE, {extent.get_primary_key()},
            action_context.m_user_context));
    }
    else {
        extent_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::UPDATE, extent, action_context.m_user_context));
    }
    CRUD_ERROR_CHECK(extent_response, action_context);
}

void HsmService::put_data(
    const HsmActionRequest& req,
    hestia::Stream* stream,
    actionCompletionFunc completion_func,
    actionProgressFunc progress_func) const
{
    HsmActionContext action_context(req, completion_func, progress_func);
    if (action_context.m_target_tier_id.empty()) {
        action_context.m_target_tier_id =
            get_tier_id(action_context.m_target_tier_prio);
    }

    auto action_get_response =
        get_or_create_action(req, action_context.m_action);
    CRUD_ERROR_CHECK(action_get_response, action_context);

    HsmObject working_object;
    get_object(action_context, working_object);
    if (action_context.m_extent.empty()) {
        action_context.m_extent = {0, stream->get_source_size()};
    }
    make_object_store_request(
        HsmObjectStoreRequestMethod::PUT, action_context, stream);
}

void HsmService::get_data(
    const HsmActionRequest& req,
    Stream* stream,
    actionCompletionFunc completion_func,
    actionProgressFunc progress_func) const
{
    HsmActionContext action_context(req, completion_func, progress_func);
    if (action_context.m_source_tier_id.empty()) {
        action_context.m_source_tier_id =
            get_tier_id(action_context.m_source_tier_prio);
    }

    auto action_response = get_or_create_action(req, action_context.m_action);
    CRUD_ERROR_CHECK(action_response, action_context);

    if (action_context.m_extent.empty()) {
        action_context.m_extent = {0, get_object_size_on_tier(action_context)};
    }
    if (action_context.m_extent.empty()) {
        const auto object_id = action_context.m_action.get_subject_key();
        std::string msg      = SOURCE_LOC() + " | Object " + object_id
                          + " not found on tier with uuid "
                          + action_context.m_source_tier_id;
        throw RequestException<HsmActionError>(
            {HsmActionErrorCode::ITEM_NOT_FOUND, msg});
    }

    make_object_store_request(
        HsmObjectStoreRequestMethod::GET, action_context, stream);
}

void HsmService::copy_data(
    const HsmActionRequest& req,
    actionCompletionFunc completion_func,
    actionProgressFunc progress_func) const
{
    HsmActionContext action_context(req, completion_func, progress_func);
    if (action_context.m_source_tier_id.empty()) {
        action_context.m_source_tier_id =
            get_tier_id(action_context.m_source_tier_prio);
    }
    if (action_context.m_target_tier_id.empty()) {
        action_context.m_target_tier_id =
            get_tier_id(action_context.m_target_tier_prio);
    }

    const auto action_response =
        get_or_create_action(req, action_context.m_action);
    CRUD_ERROR_CHECK(action_response, action_context);
    if (action_context.m_extent.empty()) {
        action_context.m_extent = {0, get_object_size_on_tier(action_context)};
    }
    if (action_context.m_extent.empty()) {
        const auto object_id = action_context.m_action.get_subject_key();
        std::string msg      = SOURCE_LOC() + " | Object " + object_id
                          + " not found on tier with uuid "
                          + action_context.m_source_tier_id;
        throw RequestException<HsmActionError>(
            {HsmActionErrorCode::ITEM_NOT_FOUND, msg});
    }
    make_object_store_request(
        HsmObjectStoreRequestMethod::COPY, action_context);
}

void HsmService::move_data(
    const HsmActionRequest& req,
    actionCompletionFunc completion_func,
    actionProgressFunc progress_func) const
{
    HsmActionContext action_context(req, completion_func, progress_func);
    if (action_context.m_source_tier_id.empty()) {
        action_context.m_source_tier_id =
            get_tier_id(action_context.m_source_tier_prio);
    }
    if (action_context.m_target_tier_id.empty()) {
        action_context.m_target_tier_id =
            get_tier_id(action_context.m_target_tier_prio);
    }

    const auto action_response =
        get_or_create_action(req, action_context.m_action);
    CRUD_ERROR_CHECK(action_response, action_context);
    if (action_context.m_extent.empty()) {
        action_context.m_extent = {0, get_object_size_on_tier(action_context)};
    }
    if (action_context.m_extent.empty()) {
        const auto object_id = action_context.m_action.get_subject_key();
        std::string msg      = SOURCE_LOC() + " | Object " + object_id
                          + " not found on tier "
                          + action_context.m_source_tier_id;
        throw RequestException<HsmActionError>(
            {HsmActionErrorCode::ITEM_NOT_FOUND, msg});
    }
    make_object_store_request(
        HsmObjectStoreRequestMethod::MOVE, action_context);
}

void HsmService::release_data(
    const HsmActionRequest& req,
    actionCompletionFunc completion_func,
    actionProgressFunc progress_func) const
{
    HsmActionContext action_context(req, completion_func, progress_func);
    if (action_context.m_source_tier_id.empty()) {
        action_context.m_source_tier_id =
            get_tier_id(action_context.m_source_tier_prio);
    }

    auto action_response = get_or_create_action(req, action_context.m_action);
    CRUD_ERROR_CHECK(action_response, action_context);
    if (action_context.m_extent.empty()) {
        action_context.m_extent = {0, get_object_size_on_tier(action_context)};
    }
    if (action_context.m_extent.empty()) {
        const auto object_id = action_context.m_action.get_subject_key();
        std::string msg      = SOURCE_LOC() + " | Object " + object_id
                          + " not found on tier "
                          + action_context.m_source_tier_id;
        throw RequestException<HsmActionError>(
            {HsmActionErrorCode::ITEM_NOT_FOUND, msg});
    }
    make_object_store_request(
        HsmObjectStoreRequestMethod::REMOVE, action_context);
}

std::size_t HsmService::get_object_size_on_tier(
    const HsmActionContext& action_context) const
{
    HsmObject working_object;
    get_object(action_context, working_object);
    for (const auto& tier_extent : working_object.tiers()) {
        if (action_context.m_source_tier_id == tier_extent.get_tier_id()) {
            return tier_extent.get_size();
        }
    }
    return 0;
}

void HsmService::create_or_update_extent(
    const HsmActionContext& action_context,
    const TierExtents& extent,
    bool do_create) const
{
    auto extent_service  = m_services->get_service(HsmItem::Type::EXTENT);
    const auto op_string = do_create ? "create" : "update";

    LOG_INFO("Doing extent "
                 << op_string << " with object id: " << extent.get_object_id()
                 << " and tier id " << extent.get_tier_id() << std::endl;);

    const auto response =
        extent_service->make_request(TypedCrudRequest<TierExtents>(
            do_create ? CrudMethod::CREATE : CrudMethod::UPDATE, extent,
            action_context.m_user_context));
    if (!response->ok()) {
        throw RequestException<HsmActionError>(
            {HsmActionErrorCode::CRUD_ERROR,
             SOURCE_LOC() + " | Creating or updating extent. \n"
                 + response->get_error().to_string()});
    }
}

void HsmService::update_object(
    const HsmActionContext& action_context, const HsmObject& object) const
{
    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);
    auto object_put_response =
        object_service->make_request(TypedCrudRequest<HsmObject>{
            CrudMethod::UPDATE, object, action_context.m_user_context});
    CRUD_ERROR_CHECK(object_put_response, action_context);
}

const std::string& HsmService::get_tier_id(uint8_t tier) const
{
    if (const auto& iter = m_tier_cache.find(tier);
        iter != m_tier_cache.end()) {
        return iter->second;
    }
    THROW("Failed to find tier: " + std::to_string(tier) + " in cache");
}

void HsmService::get_action(
    const HsmActionContext& action_context, HsmAction& action) const
{
    auto action_service    = get_service(HsmItem::Type::ACTION);
    const auto action_read = action_service->make_request(CrudRequest{
        CrudMethod::READ,
        CrudQuery{
            CrudIdentifier(action_context.m_action.get_primary_key()),
            CrudQuery::BodyFormat::ITEM},
        action_context.m_user_context});
    if (!action_read->ok()) {
        THROW("Failed to read action for error assignment");
    }
    if (!action_read->found()) {
        THROW("Failed to get action for error assignment");
    }
    action = *action_read->get_item_as<HsmAction>();
}

void HsmService::update_action(
    const HsmActionContext& action_context, const HsmAction& action) const
{
    auto action_service = get_service(HsmItem::Type::ACTION);
    const auto action_update =
        action_service->make_request(TypedCrudRequest<HsmAction>{
            CrudMethod::UPDATE, action, action_context.m_user_context});
    if (!action_update->ok()) {
        THROW("Failed to update action for error assignment");
    }
}

void HsmService::set_action_error(
    const HsmActionContext& action_context, const std::string& message) const
{
    LOG_INFO("Action error");

    HsmAction working_action;
    get_action(action_context, working_action);
    working_action.on_error(message);
    update_action(action_context, working_action);
}

void HsmService::set_action_finished_ok(
    const HsmActionContext& action_context, std::size_t bytes) const
{
    LOG_INFO("Action finished ok");

    HsmAction working_action;
    get_action(action_context, working_action);
    working_action.on_finished_ok(bytes);
    update_action(action_context, working_action);
}

void HsmService::set_action_progress(
    const HsmActionContext& action_context, std::size_t bytes) const
{
    LOG_INFO("Action progress");

    HsmAction working_action;
    get_action(action_context, working_action);
    working_action.set_num_transferred(bytes);
    update_action(action_context, working_action);

    auto response =
        HsmActionResponse::create(action_context.m_req, working_action);
    action_context.m_progress_func(std::move(response));
}

}  // namespace hestia
