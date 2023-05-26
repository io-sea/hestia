#pragma once

#include "HsmNodeService.h"

#include <memory>
#include <string>
#include <vector>

namespace hestia {
class HsmService;
class KeyValueStoreClient;
class HttpClient;

class DistributedHsmServiceConfig {
  public:
    HsmNode m_self;
    std::string m_controller_address;
};

class DistributedHsmService {
  public:
    DistributedHsmService(
        DistributedHsmServiceConfig config,
        HsmService* hsm_service,
        std::unique_ptr<HsmNodeService> node_service);

    HsmService* get_hsm_service();

    void register_self();

    void get(std::vector<HsmNode>& nodes) const;

    void put(const HsmNode& node) const;

  private:
    DistributedHsmServiceConfig m_config;
    HsmService* m_hsm_service;

    std::unique_ptr<HsmNodeService> m_node_service;
};

}  // namespace hestia
