#include "HsmService.h"

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "HsmObjectStoreClient.h"

#include "DatasetService.h"
#include "ObjectService.h"
#include "TierService.h"
#include "UuidUtils.h"

#include "HsmActionAdapter.h"
#include "StringAdapter.h"

#include "Logger.h"

#include "KeyValueStoreClient.h"
#include "Metadata.h"

#include <iostream>

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
    std::unique_ptr<DatasetService> dataset_service,
    HsmObjectStoreClient* object_store,
    std::unique_ptr<DataPlacementEngine> placement_engine,
    std::unique_ptr<EventFeed> event_feed) :
    m_object_service(std::move(object_service)),
    m_tier_service(std::move(tier_service)),
    m_dataset_service(std::move(dataset_service)),
    m_object_store(object_store),
    m_placement_engine(std::move(placement_engine)),
    m_action_adapter(HsmActionAdapter::create()),
    m_event_feed(std::move(event_feed))
{
    LOG_INFO("Creating HsmService");
}

HsmService::Ptr HsmService::create(
    KeyValueStoreClient* client, HsmObjectStoreClient* object_store)
{
    auto object_service = ObjectService::create(ObjectServiceConfig(), client);
    auto tier_service   = TierService::create(TierServiceConfig(), client);
    auto dataset_service =
        DatasetService::create(DatasetServiceConfig(), client);
    auto placement_engine =
        std::make_unique<BasicDataPlacementEngine>(tier_service.get());

    return create(
        std::move(object_service), std::move(tier_service),
        std::move(dataset_service), object_store, std::move(placement_engine));
}

HsmService::Ptr HsmService::create(
    std::unique_ptr<ObjectService> object_service,
    std::unique_ptr<TierService> tier_service,
    std::unique_ptr<DatasetService> dataset_service,
    HsmObjectStoreClient* object_store,
    std::unique_ptr<DataPlacementEngine> placement_engine,
    std::unique_ptr<EventFeed> event_feed)
{
    return std::make_unique<HsmService>(
        std::move(object_service), std::move(tier_service),
        std::move(dataset_service), object_store, std::move(placement_engine),
        std::move(event_feed));
}


HsmService::~HsmService()
{
    LOG_INFO("Destroying HsmService");
}

TierService* HsmService::get_tier_service()
{
    return m_tier_service.get();
}

ObjectService* HsmService::get_object_service()
{
    return m_object_service.get();
}

DatasetService* HsmService::get_dataset_service()
{
    return m_dataset_service.get();
}

HsmServiceResponse::Ptr HsmService::make_request(
    const HsmServiceRequest& req, hestia::Stream* stream) noexcept
{
    switch (req.method()) {
        case HsmServiceRequestMethod::CREATE:
            return create(req);
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
            return list_objects(req);
        case HsmServiceRequestMethod::LIST_TIERS:
            return list_tiers(req);
        case HsmServiceRequestMethod::LIST_ATTRIBUTES:
            return list_attributes(req);
        default:
            return nullptr;
    }
}

HsmServiceResponse::Ptr HsmService::create(
    const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService CREATE: " + req.to_string());

    auto put_response =
        m_object_service->make_request({req.object(), CrudMethod::PUT});
    ERROR_CHECK(put_response);

    auto response = HsmServiceResponse::create(req, std::move(put_response));
    LOG_INFO("Finished HSMService CREATE");

    return response;
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

    // Create or Update object
    auto put_response =
        m_object_service->make_request({req.object(), CrudMethod::PUT});
    ERROR_CHECK(put_response);

    if (stream != nullptr) {

        HsmObject working_object(put_response->item());

        const auto chosen_tier = m_placement_engine->choose_tier(
            working_object.object().m_size, req.target_tier());

        HsmObjectStoreRequest data_put_request(
            working_object.object(), HsmObjectStoreRequestMethod::PUT);
        data_put_request.set_target_tier(chosen_tier);
        data_put_request.set_extent(req.extent());

        auto data_put_response =
            m_object_store->make_request(data_put_request, stream);
        ERROR_CHECK(data_put_response);

        working_object.add_extent(chosen_tier, req.extent());

        auto object_put_response =
            m_object_service->make_request({working_object, CrudMethod::PUT});
        auto response =
            HsmServiceResponse::create(req, std::move(object_put_response));

        LOG_INFO("Finished HSMService PUT");

        add_put_event(working_object, chosen_tier);
        return response;
    }

    auto response = HsmServiceResponse::create(req, std::move(put_response));
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
            req.object().object(), HsmObjectStoreRequestMethod::GET);
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

    HsmObjectStoreRequest copy_data_request(
        req.object().object(), HsmObjectStoreRequestMethod::COPY);
    copy_data_request.set_extent(req.extent());
    copy_data_request.set_source_tier(req.source_tier());
    copy_data_request.set_target_tier(req.target_tier());

    auto copy_data_response = m_object_store->make_request(copy_data_request);
    ERROR_CHECK(copy_data_response);
    LOG_INFO("Finished HSMService COPY - COPY DATA");

    auto get_response =
        m_object_service->make_request({req.object(), CrudMethod::GET});
    ERROR_CHECK(get_response);

    auto hsm_object = get_response->item();
    hsm_object.add_extent(req.target_tier(), req.extent());

    // Should also update last modified time here
    auto put_response =
        m_object_service->make_request({hsm_object, CrudMethod::PUT});
    ERROR_CHECK(put_response);

    LOG_INFO("Finished HSMService COPY - PUT METADATA");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = UuidUtils::to_string(req.object().id());
        event.m_length      = req.object().object().m_size;
        event.m_method      = EventFeed::Event::Method::COPY;
        event.m_source_tier = req.source_tier();
        event.m_target_tier = req.target_tier();
        m_event_feed->log_event(event);
    }

    return HsmServiceResponse::create(req, std::move(put_response));
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

    HsmObjectStoreRequest move_request(
        req.object().object(), HsmObjectStoreRequestMethod::MOVE);
    move_request.set_extent(req.extent());
    move_request.set_source_tier(req.source_tier());
    move_request.set_target_tier(req.target_tier());

    auto move_data_response = m_object_store->make_request(move_request);
    ERROR_CHECK(move_data_response);
    LOG_INFO("Finished HSMService MOVE - MOVE DATA");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = UuidUtils::to_string(req.object().id());
        event.m_length      = req.object().object().m_size;
        event.m_method      = EventFeed::Event::Method::MOVE;
        event.m_source_tier = req.source_tier();
        event.m_target_tier = req.target_tier();
        m_event_feed->log_event(event);
    }

    auto get_response =
        m_object_service->make_request({req.object(), CrudMethod::GET});
    ERROR_CHECK(get_response);

    auto hsm_object = get_response->item();

    hsm_object.remove_extent(req.source_tier(), req.extent());
    hsm_object.add_extent(req.target_tier(), req.extent());

    auto put_response =
        m_object_service->make_request({hsm_object, CrudMethod::PUT});

    LOG_INFO("Finished HSMService MOVE - PUT METADATA");
    return HsmServiceResponse::create(req, std::move(put_response));
}

HsmServiceResponse::Ptr HsmService::remove(
    const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService REMOVE: " + req.to_string());

    HsmObjectStoreRequest remove_data_request(
        req.object().object(), HsmObjectStoreRequestMethod::REMOVE);
    remove_data_request.set_extent(req.extent());
    remove_data_request.set_source_tier(req.source_tier());

    auto remove_data_response =
        m_object_store->make_request(remove_data_request);
    ERROR_CHECK(remove_data_response);

    auto obj_get_request =
        m_object_service->make_request({req.object(), CrudMethod::GET});

    auto hsm_object = obj_get_request->item();
    hsm_object.remove_extent(req.source_tier(), req.extent());

    auto object_put_response =
        m_object_service->make_request({hsm_object, CrudMethod::PUT});

    LOG_INFO("Finished HSMService REMOVE");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = UuidUtils::to_string(req.object().id());
        event.m_method      = EventFeed::Event::Method::REMOVE;
        event.m_source_tier = req.source_tier();
        ;
        m_event_feed->log_event(event);
    }

    return HsmServiceResponse::create(req, std::move(object_put_response));
}

HsmServiceResponse::Ptr HsmService::remove_all(
    const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService REMOVE_ALL: " + req.to_string());


    auto obj_get_request =
        m_object_service->make_request({req.object(), CrudMethod::GET});

    auto hsm_object = obj_get_request->item();
    for (const auto& [tier_id, extents] : hsm_object.tiers()) {
        HsmObjectStoreRequest remove_data_request(
            req.object().object(), HsmObjectStoreRequestMethod::REMOVE);
        remove_data_request.set_extent(req.extent());
        remove_data_request.set_source_tier(tier_id);

        auto remove_data_response =
            m_object_store->make_request(remove_data_request);
        ERROR_CHECK(remove_data_response);
    }

    hsm_object.remove_all_tiers();

    auto object_put_response =
        m_object_service->make_request({hsm_object, CrudMethod::REMOVE});

    LOG_INFO("Finished HSMService REMOVE");

    if (m_event_feed) {
        EventFeed::Event event;
        event.m_id          = UuidUtils::to_string(req.object().id());
        event.m_method      = EventFeed::Event::Method::REMOVE_ALL;
        event.m_source_tier = req.source_tier();
        ;
        m_event_feed->log_event(event);
    }

    return HsmServiceResponse::create(req, std::move(object_put_response));
}

HsmServiceResponse::Ptr HsmService::list_objects(
    const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService LIST_OBJECTS");

    auto response = HsmServiceResponse::create(req);

    ObjectServiceRequest list_objects_request(CrudMethod::LIST);
    auto list_objects_response =
        m_object_service->make_request(list_objects_request);
    ERROR_CHECK(list_objects_response);

    for (const auto& object_id : list_objects_response->ids()) {
        HsmObject hsm_object(object_id);
        ObjectServiceRequest get_object_request(hsm_object, CrudMethod::GET);
        auto get_object_response =
            m_object_service->make_request(get_object_request);
        ERROR_CHECK(get_object_response);
        for (const auto& tier : get_object_response->item().tiers()) {
            if (req.tier() == std::stoi(std::to_string(tier.first))) {
                response->add_object(get_object_response->item());
                break;
            }
        }
    }

    LOG_INFO("Finished HSMService LIST_OBJECTS");

    return response;
}

HsmServiceResponse::Ptr HsmService::list_tiers(
    const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService LIST_TIERS");

    auto obj_get_response =
        m_object_service->make_request({req.object(), CrudMethod::GET});
    auto all_tiers_response =
        m_tier_service->make_request(CrudMethod::MULTI_GET);

    auto list_response =
        std::make_unique<CrudResponse<StorageTier, CrudErrorCode>>(req);
    for (const auto& [tier_id, tier] : obj_get_response->item().tiers()) {
        for (const auto& tier : all_tiers_response->items()) {
            if (tier_id == tier.id_uint()) {
                list_response->items().push_back(tier);
                break;
            }
        }
    }
    LOG_INFO("Finished HSMService LIST_TIERS");

    return HsmServiceResponse::create(req, std::move(list_response));
}

HsmServiceResponse::Ptr HsmService::list_attributes(
    const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService LIST_ATTRIBUTES");

    auto get_response =
        m_object_service->make_request({req.object(), CrudMethod::GET});

    /*auto hsm_object = get_response->item();

    auto list_response =
        m_object_service->make_request({hsm_object, CrudMethod::LIST});

    std::cout<<"i: " << list_response->item().metadata()<< std::endl;*/
    // for (const auto& attr : obj_get_response->item().metadata()) {

    //}

    LOG_INFO("Finished HSMService LIST_ATTRIBUTES");

    return HsmServiceResponse::create(req, std::move(get_response));
}

void HsmService::add_put_event(const HsmObject& obj, uint8_t tier)
{
    if (!m_event_feed) {
        return;
    }

    EventFeed::Event event;
    event.m_id          = UuidUtils::to_string(obj.id());
    event.m_length      = obj.object().m_size;
    event.m_method      = EventFeed::Event::Method::PUT;
    event.m_target_tier = tier;
    m_event_feed->log_event(event);
}


}  // namespace hestia
