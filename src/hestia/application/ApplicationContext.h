#pragma once

#include <memory>

namespace hestia {
class DistributedHsmService;
class KeyValueStoreClient;
class HttpClient;
class HsmObjectStoreClient;
class UserService;
class NamespaceService;

class ApplicationContext {
  public:
    void clear();

    static ApplicationContext& get();

    DistributedHsmService* get_hsm_service() const;

    KeyValueStoreClient* get_kv_store_client() const;

    HsmObjectStoreClient* get_object_store_client() const;

    HttpClient* get_http_client() const;

    UserService* get_user_service() const;

    NamespaceService* get_namespace_service() const;

    void set_hsm_service(std::unique_ptr<DistributedHsmService> hsm_service);

    void set_kv_store_client(
        std::unique_ptr<KeyValueStoreClient> kv_store_client);

    void set_object_store_client(
        std::unique_ptr<HsmObjectStoreClient> object_store_client);

    void set_http_client(std::unique_ptr<HttpClient> http_client);

    void set_user_service(std::unique_ptr<UserService> user_service);

    void set_namespace_service(
        std::unique_ptr<NamespaceService> namespace_service);

  private:
    std::unique_ptr<KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<HttpClient> m_http_client;
    std::unique_ptr<HsmObjectStoreClient> m_object_store_client;
    std::unique_ptr<DistributedHsmService> m_hsm_service;
    std::unique_ptr<UserService> m_user_service;
    std::unique_ptr<NamespaceService> m_namespace_service;
};
}  // namespace hestia