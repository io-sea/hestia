#pragma once

#include "HestiaNodeService.h"

#include <memory>
#include <string>
#include <vector>

namespace hestia {
class HsmService;
class KeyValueStoreClient;
class HttpClient;

class HestiaServiceConfig {
  public:
    std::string m_controller_address;
};

class HestiaService {
  public:
    HestiaService(
        HestiaServiceConfig config,
        HsmService* hsm_service,
        std::unique_ptr<HestiaNodeService> node_service);

    HsmService* get_hsm_service();

    void get(std::vector<HestiaNode>& nodes) const;

    void put(const HestiaNode& node) const;

  private:
    HestiaServiceConfig m_config;
    HsmService* m_hsm_service;

    std::unique_ptr<HestiaNodeService> m_node_service;
};

}  // namespace hestia
