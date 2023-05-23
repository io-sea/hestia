#pragma once

#include "HsmObject.h"
#include "ObjectServiceRequest.h"
#include "StorageTier.h"

#include <memory>

namespace hestia {
class KeyValueStoreClient;
class HttpClient;
class HsmObjectAdapter;

struct ObjectServiceConfig {
    std::string m_endpoint;
};

class ObjectService {
  public:
    using Ptr = std::unique_ptr<ObjectService>;
    ObjectService(
        KeyValueStoreClient* kv_store_client,
        HttpClient* http_client                          = nullptr,
        std::unique_ptr<HsmObjectAdapter> object_adatper = nullptr);

    virtual ~ObjectService();

    [[nodiscard]] ObjectServiceResponse::Ptr make_request(
        const ObjectServiceRequest& request) const noexcept;

  private:
    void get(HsmObject& object) const;

    void put(const HsmObject& object) const;

    bool exists(const HsmObject& object) const;

    void remove(const HsmObject& object) const;

    void list(std::vector<HsmObject>& objects) const;

    void on_exception(
        const ObjectServiceRequest& request,
        ObjectServiceResponse* response,
        const std::string& message = {}) const;

    void on_exception(
        const ObjectServiceRequest& request,
        ObjectServiceResponse* response,
        const ObjectServiceError& error) const;

    KeyValueStoreClient* m_kv_store_client{nullptr};
    HttpClient* m_http_client{nullptr};
    std::unique_ptr<HsmObjectAdapter> m_object_adapter;
};
}  // namespace hestia
