#pragma once

#include "HsmObject.h"
#include "KeyValueCrudService.h"

#include <memory>

namespace hestia {
class KeyValueStoreClient;
class HttpClient;
class HsmObjectAdapter;

struct ObjectServiceConfig {
    std::string m_endpoint;
};

class ObjectService : public KeyValueCrudService<HsmObject> {
  public:
    using Ptr = std::unique_ptr<ObjectService>;
    ObjectService(
        const ObjectServiceConfig& config,
        KeyValueStoreClient* kv_store_client,
        HttpClient* http_client                          = nullptr,
        std::unique_ptr<HsmObjectAdapter> object_adatper = nullptr);

    virtual ~ObjectService();

    void to_string(const HsmObject& item, std::string& output) const override;

    void from_string(const std::string& output, HsmObject& item) const override;

    HttpClient* m_http_client{nullptr};
    std::unique_ptr<HsmObjectAdapter> m_object_adapter;
};
}  // namespace hestia
