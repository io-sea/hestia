#pragma once

#include <memory>

namespace hestia {
class HsmService;
class KeyValueStoreClient;
class HsmObjectStoreClient;

class ApplicationContext {
  public:
    void clear();

    static ApplicationContext& get();

    HsmService* get_hsm_service() const;

    KeyValueStoreClient* get_kv_store_client() const;

    void set_hsm_service(std::unique_ptr<HsmService> hsm_service);

    void set_kv_store_client(
        std::unique_ptr<KeyValueStoreClient> kv_store_client);

    void set_object_store_client(
        std::unique_ptr<HsmObjectStoreClient> object_store_client);

  private:
    std::unique_ptr<KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<HsmObjectStoreClient> m_object_store_client;
    std::unique_ptr<HsmService> m_hsm_service;
};
}  // namespace hestia