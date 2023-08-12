#include "HsmServicesFactory.h"

#include "IdGenerator.h"
#include "TimeProvider.h"

#include "CrudResponse.h"
#include "CrudServiceBackend.h"
#include "CrudServiceFactory.h"
#include "CrudServiceWithUser.h"
#include "TypedCrudRequest.h"

#include "EventFeed.h"
#include "UserService.h"

#include "Dataset.h"
#include "HsmAction.h"
#include "HsmNode.h"
#include "HsmObject.h"
#include "Namespace.h"
#include "ObjectStoreBackend.h"
#include "StorageTier.h"
#include "TierExtents.h"
#include "UserMetadata.h"

#include <cassert>
#include <stdexcept>

namespace hestia {
CrudService::Ptr HsmServicesFactory::create_service(
    HsmItem::Type type,
    const ServiceConfig& config_in,
    CrudServiceBackend* backend,
    UserService* user_service,
    EventFeed* event_feed)
{
    ServiceConfig config = config_in;
    config.m_item_prefix = HsmItem::to_name(type);

    // Cortx Motr restriction;
    uint64_t object_minimum_id = 0x100000ULL;

    switch (type) {
        case HsmItem::Type::OBJECT:
            return CrudServiceFactory<HsmObject>::create(
                config, backend, event_feed, nullptr,
                std::make_unique<DefaultIdGenerator>(object_minimum_id));
        case HsmItem::Type::DATASET:
            return CrudServiceFactory<Dataset>::create(
                config, backend, event_feed, user_service);
        case HsmItem::Type::ACTION:
            return CrudServiceFactory<HsmAction>::create(
                config, backend, event_feed, user_service);
        case HsmItem::Type::NAMESPACE:
            return CrudServiceFactory<Namespace>::create(
                config, backend, event_feed);
        case HsmItem::Type::TIER:
            return CrudServiceFactory<StorageTier>::create(
                config, backend, event_feed);
        case HsmItem::Type::EXTENT:
            return CrudServiceFactory<TierExtents>::create(
                config, backend, event_feed);
        case HsmItem::Type::METADATA:
            return CrudServiceFactory<UserMetadata>::create(
                config, backend, event_feed);
        case HsmItem::Type::NODE:
            return CrudServiceFactory<HsmNode>::create(
                config, backend, event_feed);
        case HsmItem::Type::OBJECT_STORE_BACKEND:
            return CrudServiceFactory<ObjectStoreBackend>::create(
                config, backend, event_feed);
        case HsmItem::Type::UNKNOWN:
        default:
            return nullptr;
    }
}

HsmServiceCollection::~HsmServiceCollection() {}

void HsmServiceCollection::create_default_services(
    const ServiceConfig& config,
    CrudServiceBackend* backend,
    UserService* user_service,
    EventFeed* event_feed)
{
    for (const auto& item : HsmItem::get_all_items()) {

        auto config_in = config;
        add_service(
            item, HsmServicesFactory::create_service(
                      item, config_in, backend, user_service, event_feed));
    }

    get_service(HsmItem::Type::DATASET)
        ->register_parent_service(User::get_type(), user_service);
    get_service(HsmItem::Type::ACTION)
        ->register_parent_service(User::get_type(), user_service);
    get_service(HsmItem::Type::NODE)
        ->register_parent_service(User::get_type(), user_service);

    get_service(HsmItem::Type::OBJECT)
        ->register_parent_service(
            HsmItem::dataset_name, get_service(HsmItem::Type::DATASET));
    get_service(HsmItem::Type::OBJECT)
        ->register_child_service(
            HsmItem::user_metadata_name, get_service(HsmItem::Type::METADATA));

    get_service(HsmItem::Type::NAMESPACE)
        ->register_parent_service(
            HsmItem::dataset_name, get_service(HsmItem::Type::DATASET));

    get_service(HsmItem::Type::METADATA)
        ->register_parent_service(
            HsmItem::hsm_object_name, get_service(HsmItem::Type::OBJECT));
    get_service(HsmItem::Type::EXTENT)
        ->register_parent_service(
            HsmItem::hsm_object_name, get_service(HsmItem::Type::EXTENT));
}

void HsmServiceCollection::add_service(
    HsmItem::Type type, CrudServicePtr service)
{
    m_services[type] = std::move(service);
}

CrudService* HsmServiceCollection::get_service(HsmItem::Type type) const
{
    if (auto iter = m_services.find(type); iter != m_services.end()) {
        return iter->second.get();
    }
    throw std::runtime_error("Requested service not found");
}

}  // namespace hestia