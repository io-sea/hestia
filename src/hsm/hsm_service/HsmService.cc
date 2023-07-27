#include "HsmService.h"

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "HsmObjectStoreClient.h"

#include "CrudClient.h"
#include "CrudService.h"
#include "CrudServiceBackend.h"
#include "IdGenerator.h"
#include "StringAdapter.h"
#include "TimeProvider.h"
#include "TypedCrudRequest.h"
#include "UuidUtils.h"

#include "HsmObject.h"

#include "KeyValueStoreClient.h"
#include "Logger.h"

#include <iostream>

#define ERROR_CHECK(response)                                                  \
    if (!response->ok()) {                                                     \
        return hestia::HsmActionResponse::create(req, std::move(response));    \
    }

#define CRUD_ERROR_CHECK(response)                                             \
    if (!response->ok()) {                                                     \
        auto response = HsmActionResponse::create(req);                        \
        response->on_error(                                                    \
            {HsmActionErrorCode::ERROR, response->get_error().to_string()});   \
    }


#define ON_ERROR(code, message)                                                \
    auto response = hestia::HsmActionResponse::create(req);                    \
    response->on_error({hestia::HsmActionErrorCode::code, msg});               \
    return response;

namespace hestia {

void add_put_event(EventFeed* feed, const HsmObject& obj, uint8_t tier)
{
    if (feed == nullptr) {
        return;
    }
    EventFeed::Event event;
    event.m_id          = obj.get_primary_key();
    event.m_length      = obj.size();
    event.m_method      = EventFeed::Event::Method::PUT;
    event.m_target_tier = tier;
    feed->log_event(event);
}

HsmService::HsmService(
    const ServiceConfig& config,
    HsmServiceCollection::Ptr service_collection,
    HsmObjectStoreClient* object_store,
    std::unique_ptr<DataPlacementEngine> placement_engine,
    std::unique_ptr<EventFeed> event_feed) :
    CrudService(config),
    m_services(std::move(service_collection)),
    m_object_store(object_store),
    m_placement_engine(std::move(placement_engine)),
    m_event_feed(std::move(event_feed))
{
    LOG_INFO("Creating HsmService");
}

HsmService::Ptr HsmService::create(
    const ServiceConfig& config,
    KeyValueStoreClient* client,
    HsmObjectStoreClient* object_store,
    UserService* user_service)
{
    KeyValueStoreCrudServiceBackend backend(client);

    auto services = std::make_unique<HsmServiceCollection>();
    services->create_default_services(config, &backend, user_service);

    auto placement_engine = std::make_unique<BasicDataPlacementEngine>(
        services->get_service(HsmItem::Type::TIER));

    return std::make_unique<HsmService>(
        config, std::move(services), object_store, std::move(placement_engine));
}

HsmService::~HsmService()
{
    LOG_INFO("Destroying HsmService");
}

CrudService* HsmService::get_service(HsmItem::Type type)
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

CrudResponse::Ptr HsmService::crud_create(
    HsmItem::Type subject_type, const CrudRequest& req) const noexcept
{
    LOG_INFO("Starting HSMService CREATE");

    const auto service = m_services->get_service(subject_type);
    auto response      = service->make_request(req);

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

    if (subject_type == HsmItem::Type::OBJECT && m_event_feed) {
        EventFeed::Event event;
        event.m_id     = req.get_id();
        event.m_method = EventFeed::Event::Method::REMOVE_ALL;
        m_event_feed->log_event(event);
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

void HsmService::put_data(
    const HsmActionRequest& req,
    hestia::Stream* stream,
    dataIoCompletionFunc completion_func) const noexcept
{
    assert(stream != nullptr);

    LOG_INFO(
        "Starting HSMService PUT DATA: " + req.to_string() + " | "
        + req.get_action().get_subject_key());

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);

    auto get_response = object_service->make_request(CrudRequest{CrudQuery(
        req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM)});
    CRUD_ERROR_CHECK(get_response);

    const auto working_object = get_response->get_item_as<HsmObject>();

    const auto chosen_tier = m_placement_engine->choose_tier(
        working_object->size(), req.target_tier());

    HsmObjectStoreRequest data_put_request(
        working_object->id(), HsmObjectStoreRequestMethod::PUT);
    data_put_request.set_target_tier(chosen_tier);

    auto working_extent = req.extent();
    if (working_extent.empty()) {
        working_extent = {0, stream->get_source_size()};
    }
    data_put_request.set_extent(working_extent);

    auto data_put_response =
        m_object_store->make_request(data_put_request, stream);
    CRUD_ERROR_CHECK(data_put_response);

    if (stream->waiting_for_content()) {
        auto stream_complete_func =
            [this, base_req = BaseRequest(req),
             working_obj_copy = *working_object, chosen_tier, working_extent,
             completion_func](StreamState stream_state) {
                if (stream_state.ok()) {
                    this->on_put_data_complete(
                        base_req, working_obj_copy, chosen_tier, working_extent,
                        completion_func);
                }
                else {
                    auto response = HsmActionResponse::create(base_req);
                    response->on_error(
                        {HsmActionErrorCode::ERROR, stream_state.to_string()});
                    completion_func(std::move(response));
                }
            };
        stream->set_completion_func(stream_complete_func);
    }
    else {
        on_put_data_complete(
            req, *working_object, chosen_tier, working_extent, completion_func);
    }
}

void HsmService::on_put_data_complete(
    const BaseRequest& req,
    const HsmObject& working_object,
    uint8_t tier,
    const Extent& working_extent,
    dataIoCompletionFunc completion_func) const
{
    TierExtents extent;
    bool extent_needs_creation{true};
    for (const auto& tier_extent : working_object.tiers()) {
        if (tier_extent.tier() == tier) {
            extent                = tier_extent;
            extent_needs_creation = false;
            break;
        }
    }

    if (extent_needs_creation) {
        extent.set_object_id(working_object.get_primary_key());
    }
    extent.add_extent(working_extent);

    CrudResponsePtr extent_put_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent_needs_creation) {
        extent_put_response = extent_service->make_request(
            TypedCrudRequest<TierExtents>(CrudMethod::CREATE, extent));
    }
    else {
        extent_put_response = extent_service->make_request(
            TypedCrudRequest<TierExtents>(CrudMethod::UPDATE, extent));
    }

    if (!extent_put_response->ok()) {
        auto response = HsmActionResponse::create(req);
        response->on_error(
            {HsmActionErrorCode::ERROR,
             extent_put_response->get_error().to_string()});

        completion_func(std::move(response));
    }

    auto object_service      = m_services->get_service(HsmItem::Type::OBJECT);
    auto object_put_response = object_service->make_request(
        TypedCrudRequest<HsmObject>{CrudMethod::UPDATE, working_object});
    auto response = HsmActionResponse::create(req);

    LOG_INFO("Finished HSMService PUT");

    add_put_event(m_event_feed.get(), working_object, tier);

    completion_func(std::move(response));
}

void HsmService::get_data(
    const HsmActionRequest& req,
    Stream* stream,
    dataIoCompletionFunc completion_func) const noexcept
{
    LOG_INFO(
        "Starting HSMService GET DATA: " + req.to_string() + " | "
        + req.get_action().get_subject_key());

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);

    auto get_response = object_service->make_request(CrudRequest{CrudQuery(
        req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM)});

    // ERROR_CHECK(object_response);

    const auto working_object = get_response->get_item_as<HsmObject>();

    HsmObjectStoreRequest data_request(
        working_object->id(), HsmObjectStoreRequestMethod::GET);
    data_request.set_source_tier(req.source_tier());

    auto working_extent = req.extent();
    if (working_extent.empty()) {
        working_extent = {0, stream->get_source_size()};
    }
    data_request.set_extent(working_extent);

    auto data_response = m_object_store->make_request(data_request, stream);
    // ERROR_CHECK(data_response);

    if (stream->waiting_for_content()) {
        auto stream_complete_func =
            [this, base_req = BaseRequest(req),
             completion_func](StreamState stream_state) {
                if (stream_state.ok()) {
                    this->on_get_data_complete(base_req, completion_func);
                }
                else {
                    auto response = HsmActionResponse::create(base_req);
                    response->on_error(
                        {HsmActionErrorCode::ERROR, stream_state.to_string()});
                    completion_func(std::move(response));
                }
            };
    }
    else {
        on_get_data_complete(req, completion_func);
    }
}

void HsmService::on_get_data_complete(
    const BaseRequest& req, dataIoCompletionFunc completion_func) const
{
    LOG_INFO("Finished HSMService GET");
    completion_func(HsmActionResponse::create(req));
}

HsmActionResponse::Ptr HsmService::move_data(
    const HsmActionRequest& req) const noexcept
{
    LOG_INFO(
        "Starting HSMService MOVE DATA: " + req.to_string() + " | "
        + req.get_action().get_subject_key());

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);

    auto get_response = object_service->make_request(CrudRequest{CrudQuery(
        req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM)});
    // ERROR_CHECK(get_response);

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
    ERROR_CHECK(copy_data_response);

    TierExtents source_extent;
    TierExtents target_extent;
    bool extent_needs_creation{true};
    for (const auto& tier_extent : working_object->tiers()) {
        if (tier_extent.tier() == req.source_tier()) {
            source_extent = tier_extent;
        }
        if (tier_extent.tier() == req.target_tier()) {
            target_extent         = tier_extent;
            extent_needs_creation = false;
        }
    }

    if (extent_needs_creation) {
        target_extent.set_object_id(working_object->get_primary_key());
    }
    target_extent.add_extent(working_extent);
    source_extent.remove_extent(working_extent);

    CrudResponsePtr extent_put_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent_needs_creation) {
        extent_put_response = extent_service->make_request(
            TypedCrudRequest<TierExtents>(CrudMethod::CREATE, target_extent));
    }
    else {
        extent_put_response = extent_service->make_request(
            TypedCrudRequest<TierExtents>(CrudMethod::UPDATE, target_extent));
    }

    CRUD_ERROR_CHECK(extent_put_response);
    extent_put_response = extent_service->make_request(
        TypedCrudRequest<TierExtents>(CrudMethod::UPDATE, source_extent));
    CRUD_ERROR_CHECK(extent_put_response);

    auto object_put_response = object_service->make_request(
        TypedCrudRequest<HsmObject>{CrudMethod::UPDATE, *working_object});
    CRUD_ERROR_CHECK(object_put_response);
    auto response = HsmActionResponse::create(req);

    LOG_INFO("Finished HSMService MOVE DATA");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = working_object->id();
        event.m_length      = working_object->size();
        event.m_method      = EventFeed::Event::Method::COPY;
        event.m_source_tier = req.source_tier();
        event.m_target_tier = req.target_tier();
        m_event_feed->log_event(event);
    }

    return response;
}

HsmActionResponse::Ptr HsmService::copy_data(
    const HsmActionRequest& req) const noexcept
{
    LOG_INFO(
        "Starting HSMService COPY DATA: " + req.to_string() + " | "
        + req.get_action().get_subject_key());

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);
    auto get_response   = object_service->make_request(CrudRequest{CrudQuery(
        req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM)});
    CRUD_ERROR_CHECK(get_response);
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
    ERROR_CHECK(copy_data_response);

    TierExtents source_extent;
    TierExtents target_extent;
    bool extent_needs_creation{true};
    for (const auto& tier_extent : working_object->tiers()) {
        if (tier_extent.tier() == req.source_tier()) {
            source_extent = tier_extent;
        }
        if (tier_extent.tier() == req.target_tier()) {
            target_extent         = tier_extent;
            extent_needs_creation = false;
        }
    }

    if (extent_needs_creation) {
        target_extent.set_object_id(working_object->get_primary_key());
    }
    target_extent.add_extent(working_extent);

    CrudResponsePtr extent_put_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent_needs_creation) {
        extent_put_response = extent_service->make_request(
            TypedCrudRequest<TierExtents>(CrudMethod::CREATE, target_extent));
    }
    else {
        extent_put_response = extent_service->make_request(
            TypedCrudRequest<TierExtents>(CrudMethod::UPDATE, target_extent));
    }
    CRUD_ERROR_CHECK(extent_put_response);

    auto object_put_response = object_service->make_request(
        TypedCrudRequest<HsmObject>{CrudMethod::UPDATE, *working_object});
    CRUD_ERROR_CHECK(object_put_response);

    auto response = HsmActionResponse::create(req);

    LOG_INFO("Finished HSMService COPY DATA");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = working_object->id();
        event.m_length      = working_object->size();
        event.m_method      = EventFeed::Event::Method::COPY;
        event.m_source_tier = req.source_tier();
        event.m_target_tier = req.target_tier();
        m_event_feed->log_event(event);
    }

    return response;
}

HsmActionResponse::Ptr HsmService::release_data(
    const HsmActionRequest& req) const noexcept
{
    LOG_INFO("Starting HSMService RELEASE_DATA: " + req.to_string());

    auto object_service = m_services->get_service(HsmItem::Type::OBJECT);
    auto get_response   = object_service->make_request(CrudRequest{CrudQuery(
        req.get_action().get_subject_key(), CrudQuery::OutputFormat::ITEM)});
    CRUD_ERROR_CHECK(get_response);
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
    ERROR_CHECK(remove_data_response);

    TierExtents extent;
    for (const auto& tier_extent : working_object->tiers()) {
        if (tier_extent.tier() == req.source_tier()) {
            extent = tier_extent;
            break;
        }
    }
    extent.remove_extent(working_extent);

    CrudResponsePtr extent_put_response;
    auto extent_service = m_services->get_service(HsmItem::Type::EXTENT);
    if (extent.empty()) {
        extent_put_response = extent_service->make_request(
            TypedCrudRequest<TierExtents>(CrudMethod::REMOVE, extent));
    }
    else {
        extent_put_response = extent_service->make_request(
            TypedCrudRequest<TierExtents>(CrudMethod::UPDATE, extent));
    }
    CRUD_ERROR_CHECK(extent_put_response);

    auto object_put_response = object_service->make_request(
        TypedCrudRequest<HsmObject>{CrudMethod::UPDATE, *working_object});
    auto response = HsmActionResponse::create(req);

    LOG_INFO("Finished HSMService REMOVE");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = req.get_action().get_subject_key();
        event.m_method      = EventFeed::Event::Method::REMOVE;
        event.m_source_tier = req.source_tier();
        ;
        m_event_feed->log_event(event);
    }

    return response;
}

}  // namespace hestia
