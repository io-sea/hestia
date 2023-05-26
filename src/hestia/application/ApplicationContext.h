#pragma once

#include <memory>

namespace hestia {
class HsmService;
class KeyValueStoreClient;
class HttpClient;
class MultiBackendHsmObjectStoreClient;

class ApplicationContext {
  public:
    void clear();

    static ApplicationContext& get();

    HsmService* get_hsm_service() const;

    KeyValueStoreClient* get_kv_store_client() const;

    MultiBackendHsmObjectStoreClient* get_object_store_client() const;

    HttpClient* get_http_client() const;

    void set_hsm_service(std::unique_ptr<HsmService> hsm_service);

    void set_kv_store_client(
        std::unique_ptr<KeyValueStoreClient> kv_store_client);

    void set_object_store_client(
        std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store_client);

    void set_http_client(std::unique_ptr<HttpClient> http_client);

  private:
    std::unique_ptr<KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<HttpClient> m_http_client;
    std::unique_ptr<MultiBackendHsmObjectStoreClient> m_object_store_client;
    std::unique_ptr<HsmService> m_hsm_service;
};
}  // namespace hestia