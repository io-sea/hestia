#pragma once

#include "CrudService.h"
#include "HsmObject.h"

#include <memory>

namespace hestia {

class KeyValueStoreClient;
class HttpClient;

struct ObjectServiceConfig {
    std::string m_endpoint;
    std::string m_global_prefix;
    std::string m_item_prefix{"object"};
};

class ObjectService : public CrudService<HsmObject> {
  public:
    using Ptr = std::unique_ptr<ObjectService>;
    ObjectService(
        const ObjectServiceConfig& config,
        std::unique_ptr<CrudClient<HsmObject>> client);

    virtual ~ObjectService();

    static Ptr create(
        const ObjectServiceConfig& config, KeyValueStoreClient* client);

    static Ptr create(const ObjectServiceConfig& config, HttpClient* client);

  private:
    ObjectServiceConfig m_config;
};
}  // namespace hestia
