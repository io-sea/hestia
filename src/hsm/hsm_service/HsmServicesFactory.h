#pragma once

#include "CrudRequest.h"
#include "HsmItem.h"
#include "Service.h"

#include <memory>
#include <unordered_map>

namespace hestia {

class CrudService;
using CrudServicePtr = std::unique_ptr<CrudService>;
class CrudServiceBackend;
class CrudResponse;
using CrudResponsePtr = std::unique_ptr<CrudResponse>;

class UserService;
class HsmServiceCollection;

class HsmServicesFactory {
  public:
    static CrudServicePtr create_service(
        HsmItem::Type type,
        const ServiceConfig& config,
        CrudServiceBackend* backend,
        UserService* user_service);
};

class HsmServiceCollection {
  public:
    ~HsmServiceCollection();
    using Ptr = std::unique_ptr<HsmServiceCollection>;

    void create_default_services(
        const ServiceConfig& config,
        CrudServiceBackend* backend,
        UserService* user_service);

    void add_service(HsmItem::Type type, CrudServicePtr service);

    CrudService* get_service(HsmItem::Type type) const;

  private:
    std::unordered_map<HsmItem::Type, CrudServicePtr> m_services;
};


}  // namespace hestia