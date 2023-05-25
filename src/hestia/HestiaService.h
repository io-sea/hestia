#pragma once

#include <memory>
#include <string>
#include <vector>

namespace hestia {
class HsmService;
class KeyValueStoreClient;
class HttpClient;

class HestiaNode {
  public:
    bool m_is_controller{false};
    std::string m_host_address;
    std::string m_tag;
    std::string m_version;
    std::string m_app_type;
};

class ObjectStoreBackend {
  public:
    std::string m_identifier;
};

class HestiaServiceConfig {
  public:
    std::string m_controller_address;
};

class HestiaService {
  public:
    HestiaService(
        HestiaServiceConfig config,
        HsmService* hsm_service,
        std::unique_ptr<KeyValueStoreClient> kv_store,
        std::unique_ptr<HttpClient> http_client = nullptr);

    HsmService* get_hsm_service();

    const std::vector<HestiaNode> get_nodes() const;

    const std::vector<ObjectStoreBackend> get_backends() const;

  private:
    HestiaServiceConfig m_config;
    HsmService* m_hsm_service;

    std::unique_ptr<KeyValueStoreClient> m_kv_store;
    std::unique_ptr<HttpClient> m_client;

    std::vector<HestiaNode> m_nodes;
    std::vector<ObjectStoreBackend> m_backends;
};

}  // namespace hestia
