#include "HsmServicesFactory.h"

#include "IdGenerator.h"
#include "TimeProvider.h"

#include "CrudResponse.h"
#include "CrudServiceBackend.h"
#include "CrudServiceFactory.h"
#include "CrudServiceWithUser.h"
#include "TypedCrudRequest.h"
#include "UserService.h"

#include "Dataset.h"
#include "HsmAction.h"
#include "HsmEvent.h"
#include "HsmObject.h"
#include "Namespace.h"
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
    UserService* user_service)
{
    ServiceConfig config = config_in;
    config.m_item_prefix = HsmItem::to_name(type);

    // Cortx Motr restriction;
    uint64_t object_minimum_id = 0x100000ULL;

    switch (type) {
        case HsmItem::Type::OBJECT:
            return CrudServiceFactory<HsmObject>::create(
                config, backend, nullptr,
                std::make_unique<DefaultIdGenerator>(object_minimum_id));
        case HsmItem::Type::DATASET:
            return CrudServiceFactory<Dataset>::create(
                config, backend, user_service);
        case HsmItem::Type::ACTION:
            return CrudServiceFactory<HsmAction>::create(
                config, backend, user_service);
        case HsmItem::Type::EVENT:
            return CrudServiceFactory<HsmEvent>::create(config, backend);
        case HsmItem::Type::NAMESPACE:
            return CrudServiceFactory<Namespace>::create(config, backend);
        case HsmItem::Type::TIER:
            return CrudServiceFactory<StorageTier>::create(config, backend);
        case HsmItem::Type::EXTENT:
            return CrudServiceFactory<TierExtents>::create(config, backend);
        case HsmItem::Type::METADATA:
            return CrudServiceFactory<UserMetadata>::create(config, backend);
        case HsmItem::Type::UNKNOWN:
        default:
            return nullptr;
    }
}

HsmServiceCollection::~HsmServiceCollection() {}

void HsmServiceCollection::create_default_services(
    const ServiceConfig& config,
    CrudServiceBackend* backend,
    UserService* user_service)
{
    for (const auto& item : HsmItem::get_all_items()) {
        add_service(
            item, HsmServicesFactory::create_service(
                      item, config, backend, user_service));
    }
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