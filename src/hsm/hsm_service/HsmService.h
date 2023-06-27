#pragma once

#include "EventFeed.h"

#include "HsmObject.h"
#include "HsmServiceRequest.h"
#include "HsmServiceResponse.h"

#include "Stream.h"

namespace hestia {
class HsmObjectStoreClient;
class DataPlacementEngine;

class ObjectService;
using ObjectServiceRequest = CrudRequest<HsmObject>;

class TierService;
class DatasetService;

class HsmStoreInterface;
class HsmActionAdapter;
class KeyValueStoreClient;

class HsmService {
  public:
    using Ptr = std::unique_ptr<HsmService>;

    HsmService(
        std::unique_ptr<ObjectService> object_service,
        std::unique_ptr<TierService> tier_service,
        std::unique_ptr<DatasetService> dataset_service,
        HsmObjectStoreClient* object_store,
        std::unique_ptr<DataPlacementEngine> placement_engine,
        std::unique_ptr<EventFeed> event_feed = nullptr);

    static Ptr create(
        std::unique_ptr<ObjectService> object_service,
        std::unique_ptr<TierService> tier_service,
        std::unique_ptr<DatasetService> dataset_service,
        HsmObjectStoreClient* object_store,
        std::unique_ptr<DataPlacementEngine> placement_engine,
        std::unique_ptr<EventFeed> event_feed = nullptr);

    static Ptr create(
        KeyValueStoreClient* client, HsmObjectStoreClient* object_store);

    virtual ~HsmService();

    TierService* get_tier_service();

    ObjectService* get_object_service();

    DatasetService* get_dataset_service();

    [[nodiscard]] HsmServiceResponse::Ptr make_request(
        const HsmServiceRequest& request, Stream* stream = nullptr) noexcept;

  private:
    HsmServiceResponse::Ptr get(
        const HsmServiceRequest& request, Stream* stream = nullptr) noexcept;
    HsmServiceResponse::Ptr put(
        const HsmServiceRequest& request, Stream* stream = nullptr) noexcept;
    HsmServiceResponse::Ptr exists(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr copy(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr move(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr remove(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr remove_all(
        const HsmServiceRequest& request) noexcept;

    HsmServiceResponse::Ptr list_objects(
        const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr list_tiers(
        const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr list_attributes(
        const HsmServiceRequest& request) noexcept;

    std::unique_ptr<ObjectService> m_object_service;
    std::unique_ptr<TierService> m_tier_service;
    std::unique_ptr<DatasetService> m_dataset_service;

    HsmObjectStoreClient* m_object_store;
    std::unique_ptr<DataPlacementEngine> m_placement_engine;
    std::unique_ptr<HsmActionAdapter> m_action_adapter;

    std::unique_ptr<EventFeed> m_event_feed;
};
}  // namespace hestia
