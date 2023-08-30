#include "HsmService.h"

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "HsmObjectStoreClient.h"

#include "CrudClient.h"
#include "CrudService.h"
#include "CrudServiceBackend.h"
#include "TypedCrudRequest.h"

#include "IdGenerator.h"
#include "StorageTier.h"
#include "StringAdapter.h"
#include "TimeProvider.h"

#include "ErrorUtils.h"
#include "UuidUtils.h"

#include "HsmObject.h"

#include "KeyValueStoreClient.h"
#include "Logger.h"

#include <cassert>

#include <iostream>

#define ERROR_CHECK(response, working_action)                                  \
    if (!response->ok()) {                                                     \
        return hestia::HsmActionResponse::create(                              \
            req, working_action, std::move(response));                         \
    }

#define CRUD_ERROR_CHECK(response, working_action, completion_func)            \
    if (!response->ok()) {                                                     \
        auto action_response = HsmActionResponse::create(req, working_action); \
        action_response->on_error(                                             \
            {HsmActionErrorCode::ERROR, response->get_error().to_string()});   \
        completion_func(std::move(action_response));                           \
        return;                                                                \
    }

#define CRUD_ERROR_CHECK_RETURN(response, working_action)                      \
    if (!response->ok()) {                                                     \
        auto action_response = HsmActionResponse::create(req, working_action); \
        action_response->on_error(                                             \
            {HsmActionErrorCode::ERROR, response->get_error().to_string()});   \
        return action_response;                                                \
    }

#define ON_ERROR(code, message, working_action)                                \
    auto response = hestia::HsmActionResponse::create(req, working_action);    \
    response->on_error({hestia::HsmActionErrorCode::code, msg});               \
    return response;

namespace hestia {

HsmService::HsmService(
    const ServiceConfig& config,
    HsmServiceCollection::Ptr service_collection,
    HsmObjectStoreClient* object_store,
    std::unique_ptr<DataPlacementEngine> placement_engine) :
    CrudService(config),
    m_services(std::move(service_collection)),
    m_object_store(object_store),
    m_placement_engine(std::move(placement_engine))
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
    services->create_default_services(
        config, &backend, user_service, event_feed);

    auto placement_engine = std::make_unique<BasicDataPlacementEngine>(
        services->get_service(HsmItem::Type::TIER));

    return std::make_unique<HsmService>(
        config, std::move(services), object_store, std::move(placement_engine));
}

HsmService::~HsmService()
{
    LOG_INFO("Destroying HsmService");
}

CrudService* HsmService::get_service(HsmItem::Type type) const
{
    return m_services->get_service(type);
}

CrudResponse::Ptr HsmService::make_request(
    const CrudRequest& req, const std::string& type) const noexcept
{
    LOG_INFO("Request " << type << " " << req.method_as_string());
    const auto subject_type = HsmItem::from_name(type);
    switch (req.method()) {
        case CrudMethod::CREATE:
            return crud_create(subject_type, req);
        case CrudMethod::READ:
            return crud_read(subject_type, req);
        case CrudMethod::UPDATE:
            return crud_update(subject_type, req);
        case CrudMethod::IDENTIFY:
            return crud_identify(subject_type, req);
        case CrudMethod::REMOVE:
            return crud_remove(subject_type, req);
        case CrudMethod::LOCK:
        case CrudMethod::UNLOCK:
        case CrudMethod::LOCKED:
        default:
            return nullptr;
    }
}

HsmActionResponse::Ptr HsmService::make_request(
    const HsmActionRequest& req) const noexcept
{
    switch (req.method()) {
        case HsmAction::Action::COPY_DATA:
            return copy_data(req);
        case HsmAction::Action::MOVE_DATA:
            return move_data(req);
        case HsmAction::Action::RELEASE_DATA:
            return release_data(req);
        case HsmAction::Action::NONE:
        case HsmAction::Action::PUT_DATA:
        case HsmAction::Action::GET_DATA:
        case HsmAction::Action::CRUD:
        default:
            return nullptr;
    }
}

void HsmService::do_data_io_action(
    const HsmActionRequest& request,
    Stream* stream,
    dataIoCompletionFunc completion_func) const
{
    if (request.method() == HsmAction::Action::PUT_DATA) {
        put_data(request, stream, completion_func);
    }
    else {
        get_data(request, stream, completion_func);
    }
}

void HsmService::update_tiers(const std::string& user_id)
{
    auto tier_service = m_services->get_service(HsmItem::Type::TIER);
    auto get_response = tier_service->make_request(
        CrudRequest(CrudQuery{CrudQuery::OutputFormat::ITEM}, user_id));
    if (!get_response->ok()) {
        THROW_WITH_SOURCE_LOC("Failed listing tiers in cache request");
    }
    for (const auto& item : get_response->items()) {
        auto tier_item = dynamic_cast<StorageTier*>(item.get());
        m_tier_cache[tier_item->id_uint()] = tier_item->get_primary_key();
    }
}

CrudResponse::Ptr HsmService::crud_create(
    HsmItem::Type subject_type, const CrudRequest& req) const noexcept
{
    LOG_INFO("Starting HSMService CREATE");
    CrudService* service{nullptr};
    try {
        service = m_services->get_service(subject_type);
    }
    catch (const std::exception& e) {
        auto response =
            CrudResponse::create(req, HsmItem::to_name(subject_type));
        response->on_error(
            {CrudErrorCode::ERROR, SOURCE_LOC() + " | " + e.what()});
        return response;
    }
    auto response = service->make_request(req);

    LOG_INFO("Finished HSMService CREATE");
    return response;
}

CrudResponse::Ptr HsmService::crud_read(
    HsmItem::Type subject_type, const CrudRequest& req) const noexcept
{
    LOG_INFO("Starting HSMService READ");

    const auto service = m_services->get_service(subject_type);
    auto response      = service->make_request(req);

    LOG_INFO("Finished HSMService READ");
    return response;
}

CrudResponse::Ptr HsmService::crud_update(
    HsmItem::Type subject_type, const CrudRequest& req) const noexcept
{
    LOG_INFO("Starting HSMService UPDATE");

    const auto service = m_services->get_service(subject_type);
    auto response      = service->make_request(req);

    LOG_INFO("Finished HSMService UPDATE");
    return response;
}

CrudResponse::Ptr HsmService::crud_remove(
    HsmItem::Type subject_type, const CrudRequest& req) const noexcept
{
    LOG_INFO("Starting HSMService REMOVE");

    const auto service = m_services->get_service(subject_type);
    auto response      = service->make_request(req);
    if (!response->ok()) {
        return response;
    }

    LOG_INFO("Finished HSMService REMOVE");
    return response;
}

CrudResponse::Ptr HsmService::crud_identify(
    HsmItem::Type subject_type, const CrudRequest& req) const noexcept
{
    LOG_INFO("Starting HSMService Identify");

    const auto service = m_services->get_service(subject_type);
    auto response      = service->make_request(req);

    LOG_INFO("Finished HSMService Identify");
    return response;
}

CrudResponsePtr HsmService::get_or_create_action(
    const HsmActionRequest& req, HsmAction& working_action) const
{
    if (working_action.get_primary_key().empty()) {
        auto action_service = m_services->get_service(HsmItem::Type::ACTION);
        auto action_response =
            action_service->make_request(TypedCrudRequest<HsmAction>{
                CrudMethod::CREATE, working_action, req.get_user_context(),
                CrudQuery::OutputFormat::ITEM});
        if (!action_response->ok()) {
            return action_response;
        }
        working_action = *action_response->get_item_as<HsmAction>();
    }
    return CrudResponse::create(req, HsmItem::hsm_action_name);
}

void HsmService::put_data(
    const HsmActionRequest& req,
    hestia::Stream* stream,
    dataIoCompletionFunc completion_func) const noexcept
{
    assert(stream != nullptr);

    HsmAction working_action = req.get_action();
    auto action_get_response = get_or_create_action(req, working_action);
    CRUD_ERROR_CHECK(action_get_response, working_action, completion_func);

    LOG_INFO(
        "Starting HSMService PUT DATA: " + req.to_string()
            + " | Subject Id: " + req.get_action().get_subject_key()
        << " | Action Id: " << working_action.get_primary_key());

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);
    auto get_response   = object_service->make_request(CrudRequest{
        CrudQuery(
            req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM),
        req.get_user_context()});
    CRUD_ERROR_CHECK(get_response, working_action, completion_func);

    if (!get_response->found()) {
        auto response = HsmActionResponse::create(req, working_action);
        response->on_error(
            {HsmActionErrorCode::ITEM_NOT_FOUND,
             SOURCE_LOC() + " | Object " + req.get_action().get_subject_key()
                 + " not found"});
        completion_func(std::move(response));
    }

    const auto working_object = get_response->get_item_as<HsmObject>();

    auto chosen_tier = req.target_tier();
    if (m_placement_engine != nullptr) {
        chosen_tier = m_placement_engine->choose_tier(
            working_object->size(), req.target_tier());
    }

    StorageObject storage_object(working_object->id());
    storage_object.get_metadata_as_writeable().set_item(
        "hestia-user_token", req.get_user_context().m_token);

    HsmObjectStoreRequest data_put_request(
        storage_object, HsmObjectStoreRequestMethod::PUT);
    data_put_request.set_target_tier(chosen_tier);

    auto working_extent = req.extent();
    if (working_extent.empty()) {
        working_extent = {0, stream->get_source_size()};
    }
    data_put_request.set_extent(working_extent);

    auto data_put_response =
        m_object_store->make_request(data_put_request, stream);
    CRUD_ERROR_CHECK(data_put_response, working_action, completion_func);

    const auto requires_db_update = !data_put_response->object_is_remote();
    if (requires_db_update) {
        LOG_INFO("Will update db from this node");
    }
    const auto store_id = data_put_response->get_store_id();

    if (stream->waiting_for_content()) {
        LOG_INFO("Stream waiting for content");
        auto stream_complete_func =
            [this, base_req = BaseRequest(req),
             working_obj_copy = *working_object, chosen_tier, working_extent,
             user_context     = req.get_user_context(), store_id,
             requires_db_update, working_action,
             completion_func](StreamState stream_state) {
                LOG_INFO("Stream completed");
                if (stream_state.ok()) {
                    if (requires_db_update) {
                        this->on_put_data_complete(
                            base_req, user_context, working_obj_copy,
                            chosen_tier, working_extent, store_id,
                            working_action, completion_func);
                    }
                    else {
                        auto response =
                            HsmActionResponse::create(base_req, working_action);
                        completion_func(std::move(response));
                    }
                }
                else {
                    auto response =
                        HsmActionResponse::create(base_req, working_action);
                    response->on_error(
                        {HsmActionErrorCode::ERROR, stream_state.to_string()});
                    completion_func(std::move(response));
                }
            };
        stream->set_completion_func(stream_complete_func);
    }
    else {
        if (requires_db_update) {
            on_put_data_complete(
                req, req.get_user_context(), *working_object, chosen_tier,
                working_extent, store_id, working_action, completion_func);
        }
        else {
            auto response = HsmActionResponse::create(req, working_action);
            completion_func(std::move(response));
        }
    }
}

void HsmService::on_put_data_complete(
    const BaseRequest& req,
    const CrudUserContext& user_context,
    const HsmObject& working_object,
    uint8_t tier,
    const Extent& working_extent,
    const std::string& store_id,
    const HsmAction& working_action,
    dataIoCompletionFunc completion_func) const
{
    LOG_INFO("Doing db update");
    TierExtents extent;
    bool extent_needs_creation{true};
    for (const auto& tier_extent : working_object.tiers()) {
        if (tier_extent.get_tier_id() == get_tier_id(tier)) {
            extent                = tier_extent;
            extent_needs_creation = false;
            break;
        }
    }

    if (extent_needs_creation) {
        extent.set_object_id(working_object.get_primary_key());
        extent.set_tier_id(get_tier_id(tier));
        extent.set_backend_id(store_id);
    }
    extent.add_extent(working_extent);

    CrudResponsePtr extent_put_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent_needs_creation) {
        extent_put_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::CREATE, extent, user_context));
    }
    else {
        extent_put_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::UPDATE, extent, user_context));
    }

    if (!extent_put_response->ok()) {
        auto response = HsmActionResponse::create(req, working_action);
        response->on_error(
            {HsmActionErrorCode::ERROR,
             extent_put_response->get_error().to_string()});

        completion_func(std::move(response));
    }

    auto updated_object = working_object;
    if (extent.get_size() > updated_object.size()) {
        updated_object.set_size(extent.get_size());
    }

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);
    auto object_put_response =
        object_service->make_request(TypedCrudRequest<HsmObject>{
            CrudMethod::UPDATE, updated_object, user_context});
    auto response = HsmActionResponse::create(req, working_action);

    set_action_finished_ok(
        user_context, working_action.get_primary_key(),
        working_extent.m_length);

    LOG_INFO(
        "Finished HSMService PUT | Action ID: "
        + working_action.get_primary_key());

    completion_func(std::move(response));
}

const std::string& HsmService::get_tier_id(uint8_t tier) const
{
    if (const auto& iter = m_tier_cache.find(tier);
        iter != m_tier_cache.end()) {
        return iter->second;
    }
    THROW_WITH_SOURCE_LOC(
        "Failed to find tier: " + std::to_string(tier) + " in cache");
}

void HsmService::get_data(
    const HsmActionRequest& req,
    Stream* stream,
    dataIoCompletionFunc completion_func) const noexcept
{
    LOG_INFO(
        "Starting HSMService GET DATA: " + req.to_string() + " | "
        + req.get_action().get_subject_key() + " | Tier "
        + std::to_string(req.source_tier()));

    HsmAction working_action = req.get_action();
    auto action_response     = get_or_create_action(req, working_action);
    CRUD_ERROR_CHECK(action_response, working_action, completion_func);

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);

    auto get_response = object_service->make_request(CrudRequest{
        CrudQuery(
            req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM),
        req.get_user_context()});
    CRUD_ERROR_CHECK(get_response, working_action, completion_func);

    const auto working_object = get_response->get_item_as<HsmObject>();

    HsmObjectStoreRequest data_request(
        working_object->id(), HsmObjectStoreRequestMethod::GET);
    data_request.set_source_tier(req.source_tier());

    auto working_extent = req.extent();
    if (working_extent.empty()) {
        const auto tier_id = get_tier_id(req.source_tier());
        for (const auto& tier_extent : working_object->tiers()) {
            if (tier_id == tier_extent.get_tier_id()) {
                working_extent = {0, tier_extent.get_size()};
            }
        }
    }
    data_request.set_extent(working_extent);

    auto data_response = m_object_store->make_request(data_request, stream);
    // ERROR_CHECK(data_response);

    if (stream->waiting_for_content()) {
        auto stream_complete_func =
            [this, base_req = BaseRequest(req), working_action,
             completion_func](StreamState stream_state) {
                if (stream_state.ok()) {
                    this->on_get_data_complete(
                        base_req, working_action, completion_func);
                }
                else {
                    auto response =
                        HsmActionResponse::create(base_req, working_action);
                    response->on_error(
                        {HsmActionErrorCode::ERROR, stream_state.to_string()});
                    completion_func(std::move(response));
                }
            };
        stream->set_completion_func(stream_complete_func);
    }
    else {
        on_get_data_complete(req, working_action, completion_func);
    }
}

void HsmService::on_get_data_complete(
    const BaseRequest& req,
    const HsmAction& working_action,
    dataIoCompletionFunc completion_func) const
{
    LOG_INFO("Finished HSMService GET");
    completion_func(HsmActionResponse::create(req, working_action));
}

void HsmService::set_action_error(
    const CrudUserContext& user_context,
    const std::string& action_id,
    const std::string& message)
{
    auto action_service = get_service(HsmItem::Type::ACTION);

    const auto action_read = action_service->make_request(CrudRequest{
        CrudQuery{CrudIdentifier(action_id), CrudQuery::OutputFormat::ITEM},
        user_context});
    if (!action_read->ok()) {
        throw std::runtime_error("Failed to get action for error assignment");
    }

    if (!action_read->found()) {
        throw std::runtime_error("Failed to find action for error assignment");
    }

    auto action = *action_read->get_item_as<HsmAction>();
    action.on_error(message);

    const auto action_update = action_service->make_request(CrudRequest{
        CrudMethod::UPDATE, user_context, {CrudIdentifier(action_id)}});
    if (!action_update->ok()) {
        throw std::runtime_error("Failed to get action for error assignment");
    }
}

void HsmService::set_action_finished_ok(
    const CrudUserContext& user_context,
    const std::string& action_id,
    std::size_t bytes) const
{
    auto action_service = get_service(HsmItem::Type::ACTION);

    const auto action_read = action_service->make_request(CrudRequest{
        CrudQuery{CrudIdentifier(action_id), CrudQuery::OutputFormat::ITEM},
        user_context});
    if (!action_read->ok()) {
        throw std::runtime_error("Failed to get action for error assignment");
    }

    if (!action_read->found()) {
        throw std::runtime_error("Failed to find action for error assignment");
    }

    auto action = *action_read->get_item_as<HsmAction>();
    action.on_finished_ok(bytes);

    const auto action_update = action_service->make_request(CrudRequest{
        CrudMethod::UPDATE, user_context, {CrudIdentifier(action_id)}});
    if (!action_update->ok()) {
        throw std::runtime_error("Failed to get action for error assignment");
    }
}

void HsmService::set_action_progress(const std::string&, std::size_t) {}

HsmActionResponse::Ptr HsmService::move_data(
    const HsmActionRequest& req) const noexcept
{
    LOG_INFO(
        "Starting HSMService MOVE DATA: " + req.to_string() + " | "
        + req.get_action().get_subject_key());

    HsmAction working_action = req.get_action();
    auto action_response     = get_or_create_action(req, working_action);
    CRUD_ERROR_CHECK_RETURN(action_response, working_action);

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);

    auto get_response = object_service->make_request(CrudRequest{
        CrudQuery(
            req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM),
        req.get_user_context()});
    CRUD_ERROR_CHECK_RETURN(get_response, working_action);

    const auto working_object = get_response->get_item_as<HsmObject>();

    HsmObjectStoreRequest copy_data_request(
        working_object->id(), HsmObjectStoreRequestMethod::COPY);

    auto working_extent = req.extent();
    if (working_extent.empty()) {
        working_extent = {0, working_object->size()};
    }
    copy_data_request.set_extent(working_extent);
    copy_data_request.set_source_tier(req.source_tier());
    copy_data_request.set_target_tier(req.target_tier());

    auto copy_data_response = m_object_store->make_request(copy_data_request);
    ERROR_CHECK(copy_data_response, working_action);

    TierExtents source_extent;
    TierExtents target_extent;
    bool extent_needs_creation{true};
    for (const auto& tier_extent : working_object->tiers()) {
        if (tier_extent.get_tier_id() == get_tier_id(req.source_tier())) {
            source_extent = tier_extent;
        }
        if (tier_extent.get_tier_id() == get_tier_id(req.target_tier())) {
            target_extent         = tier_extent;
            extent_needs_creation = false;
        }
    }

    if (extent_needs_creation) {
        target_extent.set_object_id(working_object->get_primary_key());
        target_extent.set_tier_id(get_tier_id(req.target_tier()));
        target_extent.set_backend_id(copy_data_response->get_store_id());
    }
    target_extent.add_extent(working_extent);
    source_extent.remove_extent(working_extent);

    CrudResponsePtr extent_put_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent_needs_creation) {
        extent_put_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::CREATE, target_extent, req.get_user_context()));
    }
    else {
        extent_put_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::UPDATE, target_extent, req.get_user_context()));
    }

    CRUD_ERROR_CHECK_RETURN(extent_put_response, working_action);
    extent_put_response =
        extent_service->make_request(TypedCrudRequest<TierExtents>(
            CrudMethod::UPDATE, source_extent, req.get_user_context()));
    CRUD_ERROR_CHECK_RETURN(extent_put_response, working_action);

    auto object_put_response =
        object_service->make_request(TypedCrudRequest<HsmObject>{
            CrudMethod::UPDATE, *working_object, req.get_user_context()});
    CRUD_ERROR_CHECK_RETURN(object_put_response, working_action);
    auto response = HsmActionResponse::create(req, working_action);

    LOG_INFO("Finished HSMService MOVE DATA");

    return response;
}

HsmActionResponse::Ptr HsmService::copy_data(
    const HsmActionRequest& req) const noexcept
{
    LOG_INFO(
        "Starting HSMService COPY DATA: " + req.to_string() + " | "
        + req.get_action().get_subject_key());

    HsmAction working_action = req.get_action();
    auto action_response     = get_or_create_action(req, working_action);
    CRUD_ERROR_CHECK_RETURN(action_response, working_action);

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);
    auto get_response   = object_service->make_request(CrudRequest{
        CrudQuery(
            req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM),
        req.get_user_context()});
    CRUD_ERROR_CHECK_RETURN(get_response, working_action);

    const auto working_object = get_response->get_item_as<HsmObject>();

    HsmObjectStoreRequest copy_data_request(
        working_object->id(), HsmObjectStoreRequestMethod::COPY);

    auto working_extent = req.extent();
    if (working_extent.empty()) {
        working_extent = {0, working_object->size()};
    }
    copy_data_request.set_extent(working_extent);
    copy_data_request.set_source_tier(req.source_tier());
    copy_data_request.set_target_tier(req.target_tier());

    auto copy_data_response = m_object_store->make_request(copy_data_request);
    ERROR_CHECK(copy_data_response, working_action);

    if (copy_data_response->is_handled_remote()) {
        auto response = HsmActionResponse::create(req, working_action);
        LOG_INFO("Finished HSMService COPY DATA - Handled on remote");
        return response;
    }

    TierExtents source_extent;
    TierExtents target_extent;
    bool extent_needs_creation{true};
    for (const auto& tier_extent : working_object->tiers()) {
        if (tier_extent.get_tier_id() == get_tier_id(req.source_tier())) {
            source_extent = tier_extent;
        }
        if (tier_extent.get_tier_id() == get_tier_id(req.target_tier())) {
            target_extent         = tier_extent;
            extent_needs_creation = false;
        }
    }

    if (extent_needs_creation) {
        target_extent.set_object_id(working_object->get_primary_key());
        target_extent.set_tier_id(get_tier_id(req.target_tier()));
        target_extent.set_backend_id(copy_data_response->get_store_id());
    }
    target_extent.add_extent(working_extent);

    CrudResponsePtr extent_put_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent_needs_creation) {
        extent_put_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::CREATE, target_extent, req.get_user_context()));
    }
    else {
        extent_put_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::UPDATE, target_extent, req.get_user_context()));
    }
    CRUD_ERROR_CHECK_RETURN(extent_put_response, working_action);

    auto object_put_response =
        object_service->make_request(TypedCrudRequest<HsmObject>{
            CrudMethod::UPDATE, *working_object, req.get_user_context()});
    CRUD_ERROR_CHECK_RETURN(object_put_response, working_action);

    auto response = HsmActionResponse::create(req, working_action);

    LOG_INFO("Finished HSMService COPY DATA");

    return response;
}

HsmActionResponse::Ptr HsmService::release_data(
    const HsmActionRequest& req) const noexcept
{
    LOG_INFO("Starting HSMService RELEASE_DATA: " + req.to_string());

    HsmAction working_action = req.get_action();
    auto action_response     = get_or_create_action(req, working_action);
    CRUD_ERROR_CHECK_RETURN(action_response, working_action);

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);
    auto get_response   = object_service->make_request(CrudRequest{
        CrudQuery(
            req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM),
        req.get_user_context()});
    CRUD_ERROR_CHECK_RETURN(get_response, working_action);
    auto working_object = get_response->get_item_as<HsmObject>();

    HsmObjectStoreRequest remove_data_request(
        req.get_action().get_subject_key(),
        HsmObjectStoreRequestMethod::REMOVE);

    auto working_extent = req.extent();
    if (working_extent.empty()) {
        working_extent = {0, working_object->size()};
    }
    remove_data_request.set_extent(working_extent);
    remove_data_request.set_source_tier(req.source_tier());
    auto remove_data_response =
        m_object_store->make_request(remove_data_request);
    ERROR_CHECK(remove_data_response, working_action);

    TierExtents extent;
    for (const auto& tier_extent : working_object->tiers()) {
        if (tier_extent.get_tier_id() == get_tier_id(req.source_tier())) {
            extent = tier_extent;
            break;
        }
    }
    extent.remove_extent(working_extent);

    CrudResponsePtr extent_put_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent.empty()) {
        extent_put_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::REMOVE, extent, req.get_user_context()));
    }
    else {
        extent_put_response =
            extent_service->make_request(TypedCrudRequest<TierExtents>(
                CrudMethod::UPDATE, extent, req.get_user_context()));
    }
    CRUD_ERROR_CHECK_RETURN(extent_put_response, working_action);

    auto object_put_response =
        object_service->make_request(TypedCrudRequest<HsmObject>{
            CrudMethod::UPDATE, *working_object, req.get_user_context()});
    auto response = HsmActionResponse::create(req, working_action);

    LOG_INFO("Finished HSMService REMOVE");

    return response;
}

}  // namespace hestia
