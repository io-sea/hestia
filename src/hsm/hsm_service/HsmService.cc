#include "HsmService.h"

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "MultiBackendHsmObjectStoreClient.h"
#include "ObjectService.h"
#include "TierService.h"

#include "HsmActionAdapter.h"
#include "HsmObjectAdapter.h"

#include "Logger.h"

#define ERROR_CHECK(response)                                                  \
    if (!response->ok()) {                                                     \
        return hestia::HsmServiceResponse::create(req, std::move(response));   \
    }

#define ON_ERROR(code, message)                                                \
    auto response = hestia::HsmServiceResponse::create(req);                   \
    response->on_error({hestia::HsmServiceErrorCode::code, msg});              \
    return response;

namespace hestia {
HsmService::HsmService(
    std::unique_ptr<ObjectService> object_service,
    std::unique_ptr<TierService> tier_service,
    MultiBackendHsmObjectStoreClient* object_store,
    std::unique_ptr<DataPlacementEngine> placement_engine,
    std::unique_ptr<EventFeed> event_feed) :
    m_object_service(std::move(object_service)),
    m_tier_service(std::move(tier_service)),
    m_object_store(object_store),
    m_placement_engine(std::move(placement_engine)),
    m_action_adapter(HsmActionAdapter::create()),
    m_event_feed(std::move(event_feed))
{
    LOG_INFO("Creating HsmService");
}

HsmService::Ptr HsmService::create(
    KeyValueStoreClient* client, MultiBackendHsmObjectStoreClient* object_store)
{
    auto object_service = ObjectService::create(ObjectServiceConfig(), client);
    auto tier_service   = TierService::create(TierServiceConfig(), client);
    auto placement_engine =
        std::make_unique<BasicDataPlacementEngine>(tier_service.get());

    return create(
        std::move(object_service), std::move(tier_service), object_store,
        std::move(placement_engine));
}

HsmService::Ptr HsmService::create(
    std::unique_ptr<ObjectService> object_service,
    std::unique_ptr<TierService> tier_service,
    MultiBackendHsmObjectStoreClient* object_store,
    std::unique_ptr<DataPlacementEngine> placement_engine,
    std::unique_ptr<EventFeed> event_feed)
{
    return std::make_unique<HsmService>(
        std::move(object_service), std::move(tier_service), object_store,
        std::move(placement_engine), std::move(event_feed));
}


HsmService::~HsmService()
{
    LOG_INFO("Destroying HsmService");
}

HsmServiceResponse::Ptr HsmService::make_request(
    const HsmServiceRequest& req, hestia::Stream* stream) noexcept
{
    switch (req.method()) {
        case HsmServiceRequestMethod::GET:
            return get(req, stream);
        case HsmServiceRequestMethod::PUT:
            return put(req, stream);
        case HsmServiceRequestMethod::EXISTS:
            return exists(req);
        case HsmServiceRequestMethod::COPY:
            return copy(req);
        case HsmServiceRequestMethod::MOVE:
            return move(req);
        case HsmServiceRequestMethod::REMOVE:
            return remove(req);
        case HsmServiceRequestMethod::REMOVE_ALL:
            return remove_all(req);
        case HsmServiceRequestMethod::GET_TIERS:
        case HsmServiceRequestMethod::LIST:
        case HsmServiceRequestMethod::LIST_TIERS:
        default:
            return nullptr;
    }
}

HsmServiceResponse::Ptr HsmService::put(
    const HsmServiceRequest& req, hestia::Stream* stream) noexcept
{
    LOG_INFO("Starting HSMService PUT: " + req.to_string());

    auto exists_response = m_object_service->make_request(
        ObjectServiceRequest(req.object(), CrudMethod::EXISTS));
    ERROR_CHECK(exists_response);
    if (exists_response->found() != req.should_overwrite_put()) {
        const std::string msg =
            "Overwrite request not compatible with object existence.";
        ON_ERROR(BAD_PUT_OVERWRITE_COMBINATION, msg);
    }

    if (stream != nullptr) {
        const auto chosen_tier = m_placement_engine->choose_tier(
            req.extent().m_length, req.target_tier());

        HsmObjectStoreRequest data_put_request(
            req.object(), HsmObjectStoreRequestMethod::PUT);
        data_put_request.set_target_tier(chosen_tier);
        data_put_request.set_extent(req.extent());
        if (req.should_overwrite_put()) {
            data_put_request.object().update_modified_time();
        }
        else {
            data_put_request.object().initialize_timestamps();
        }
        auto data_put_response =
            m_object_store->make_request(data_put_request, stream);
        ERROR_CHECK(data_put_response);

        // Update metadata
        auto obj = req.object();
        HsmObject hsm_object(obj);
        hsm_object.add_tier(chosen_tier);

        ObjectServiceRequest obj_put_request(hsm_object, CrudMethod::PUT);

        auto object_put_response =
            m_object_service->make_request(obj_put_request);
        auto response =
            HsmServiceResponse::create(req, std::move(object_put_response));

        LOG_INFO("Finished HSMService PUT");

        if (m_event_feed) {
            EventFeed::Event event;
            event.m_id          = req.object().id();
            event.m_length      = req.extent().m_length;
            event.m_method      = EventFeed::Event::Method::PUT;
            event.m_target_tier = req.target_tier();
            m_event_feed->log_event(event);
        }

        return response;
    }

    if (!req.query().empty()) {
        HsmAction action;
        m_action_adapter->parse(req.query(), action);
        if (action.has_action()) {
            // First finish this request and remove the metadata
            /*
            switch (action.mAction)
            {
                case HsmAction::Action::RELEASE:
                    HsmServiceRequest action_req(req.object(),
            HsmServiceRequestMethod::REMOVE);
                    action_req.setSourceTier(action.mSourceTier);
                    return makeRequest(action_req);
                case HsmAction::Action::COPY:
                    HsmServiceRequest action_req(req.object(),
            HsmServiceRequestMethod::COPY);
                    action_req.setSourceTier(action.mSourceTier);
                    action_req.setTargetTier(action.mTargetTier);
                    return makeRequest(action_req);
                    break;
                case HsmAction::Action::MOVE:
                    HsmServiceRequest action_req(req.object(),
            HsmServiceRequestMethod::MOVE);
                    action_req.setSourceTier(action.mSourceTier);
                    action_req.setTargetTier(action.mTargetTier);
                    return makeRequest(action_req);
                    break;
                    break;
                default:
                    break;
            }
            */
        }
    }

    ObjectServiceRequest obj_put_request(req.object(), CrudMethod::PUT);
    auto object_put_response = m_object_service->make_request(obj_put_request);
    auto response =
        HsmServiceResponse::create(req, std::move(object_put_response));
    LOG_INFO("Finished HSMService PUT");

    return response;
}

HsmServiceResponse::Ptr HsmService::get(
    const HsmServiceRequest& req, hestia::Stream* stream) noexcept
{
    LOG_INFO("Starting HSMService GET: " + req.to_string());

    auto exists_response = m_object_service->make_request(
        ObjectServiceRequest(req.object(), CrudMethod::EXISTS));
    ERROR_CHECK(exists_response);
    if (!exists_response->found()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    auto object_response = m_object_service->make_request(
        ObjectServiceRequest(req.object(), CrudMethod::GET));
    ERROR_CHECK(object_response);

    if (stream != nullptr) {
        HsmObjectStoreRequest data_request(
            req.object(), HsmObjectStoreRequestMethod::GET);
        data_request.set_source_tier(req.source_tier());
        data_request.set_extent(req.extent());
        auto data_response = m_object_store->make_request(data_request, stream);
        ERROR_CHECK(data_response);
    }

    LOG_INFO("Finished HSMService GET");
    return HsmServiceResponse::create(req, std::move(object_response));
}

HsmServiceResponse::Ptr HsmService::exists(
    const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService EXISTS: " + req.to_string());

    auto exists_response = m_object_service->make_request(
        ObjectServiceRequest(req.object(), CrudMethod::EXISTS));
    ERROR_CHECK(exists_response);
    bool found    = exists_response->found();
    auto response = HsmServiceResponse::create(req, std::move(exists_response));
    response->set_object_found(found);
    return response;
}

HsmServiceResponse::Ptr HsmService::copy(const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService COPY: " + req.to_string());

    auto exists_response = m_object_service->make_request(
        ObjectServiceRequest(req.object(), CrudMethod::EXISTS));
    ERROR_CHECK(exists_response);
    if (!exists_response->found()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    auto obj = req.object();
    HsmObject hsm_object(obj);

    HsmObjectStoreRequest copy_data_request(
        req.object(), HsmObjectStoreRequestMethod::COPY);
    copy_data_request.set_extent(req.extent());
    copy_data_request.set_source_tier(req.source_tier());
    copy_data_request.set_target_tier(req.target_tier());

    auto copy_data_response = m_object_store->make_request(copy_data_request);
    ERROR_CHECK(copy_data_response);
    LOG_INFO("Finished HSMService COPY - COPY DATA");

    hsm_object.add_tier(req.target_tier());

    ObjectServiceRequest obj_put_request(req.object(), CrudMethod::PUT);
    auto object_put_response = m_object_service->make_request(obj_put_request);

    LOG_INFO("Finished HSMService COPY - PUT METADATA");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = req.object().id();
        event.m_length      = req.extent().m_length;
        event.m_method      = EventFeed::Event::Method::COPY;
        event.m_source_tier = req.source_tier();
        event.m_target_tier = req.target_tier();
        m_event_feed->log_event(event);
    }

    return HsmServiceResponse::create(req, std::move(object_put_response));
}

HsmServiceResponse::Ptr HsmService::move(const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService MOVE: " + req.to_string());

    auto exists_response = m_object_service->make_request(
        ObjectServiceRequest(req.object(), CrudMethod::EXISTS));
    ERROR_CHECK(exists_response);
    if (!exists_response->found()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    auto obj = req.object();
    HsmObject hsm_object(obj);

    HsmObjectStoreRequest move_request(
        req.object(), HsmObjectStoreRequestMethod::MOVE);
    move_request.set_extent(req.extent());
    move_request.set_source_tier(req.source_tier());
    move_request.set_target_tier(req.target_tier());

    auto move_data_response = m_object_store->make_request(move_request);
    ERROR_CHECK(move_data_response);
    LOG_INFO("Finished HSMService MOVE - MOVE DATA");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = req.object().id();
        event.m_length      = req.extent().m_length;
        event.m_method      = EventFeed::Event::Method::MOVE;
        event.m_source_tier = req.source_tier();
        event.m_target_tier = req.target_tier();
        m_event_feed->log_event(event);
    }

    hsm_object.add_tier(req.target_tier());

    ObjectServiceRequest obj_put_request(req.object(), CrudMethod::PUT);
    auto object_put_response = m_object_service->make_request(obj_put_request);

    LOG_INFO("Finished HSMService MOVE - PUT METADATA");
    return HsmServiceResponse::create(req, std::move(object_put_response));
}

HsmServiceResponse::Ptr HsmService::remove(
    const HsmServiceRequest& req) noexcept
{
    auto obj = req.object();

    HsmObject hsm_object(obj);

    HsmObjectStoreRequest remove_data_request(
        req.object(), HsmObjectStoreRequestMethod::REMOVE_ALL);
    remove_data_request.set_extent(req.extent());
    remove_data_request.set_source_tier(req.source_tier());

    auto remove_data_response =
        m_object_store->make_request(remove_data_request);
    ERROR_CHECK(remove_data_response);

    hsm_object.remove_tier(req.source_tier());

    ObjectServiceRequest obj_put_request(req.object(), CrudMethod::PUT);
    auto object_put_response = m_object_service->make_request(obj_put_request);

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = req.object().id();
        event.m_method      = EventFeed::Event::Method::REMOVE;
        event.m_source_tier = req.source_tier();
        m_event_feed->log_event(event);
    }

    return HsmServiceResponse::create(req, std::move(object_put_response));
}

HsmServiceResponse::Ptr HsmService::remove_all(
    const HsmServiceRequest& req) noexcept
{
    HsmObject hsm_object(req.object());

    HsmObjectStoreRequest removal_all_request(
        req.object(), HsmObjectStoreRequestMethod::REMOVE_ALL);
    removal_all_request.set_extent(req.extent());

    auto remove_data_response =
        m_object_store->make_request(removal_all_request);
    ERROR_CHECK(remove_data_response);

    hsm_object.remove_all_but_one_tiers();

    ObjectServiceRequest obj_put_request(req.object(), CrudMethod::PUT);
    auto object_put_response = m_object_service->make_request(obj_put_request);

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id     = req.object().id();
        event.m_method = EventFeed::Event::Method::REMOVE_ALL;
        m_event_feed->log_event(event);  // TODO: Get remaining tier
    }

    return HsmServiceResponse::create(req, std::move(object_put_response));
}

void HsmService::list_objects(uint8_t tier, std::vector<HsmObject>& objects)
{
    (void)tier;
    (void)objects;
    /*
    std::vector<hestia::StorageObject> objs;
    m_key_value_store->list(tier, objs);
    /*/
}

void HsmService::list_tiers(HsmObject& object, std::vector<uint8_t>& tiers)
{
    (void)tiers;
    (void)object;
    /*
    if (!m_key_value_store->exists(object.mStorageObject))
    {
        throw std::runtime_error("Requested non-existing object");
    }

    m_key_value_store->fetch(object.mStorageObject, {"tiers"});
    HsmObjectAdapter::parseTiers(object);
    tiers = object.mTierIds;
    */
}

void HsmService::list_attributes(HsmObject& object)
{
    (void)object;
}

}  // namespace hestia
