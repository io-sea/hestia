#pragma once

#include "CrudService.h"
#include "HsmNode.h"

#include <memory>

namespace hestia {

class KeyValueStoreClient;
class HttpClient;

struct HsmNodeServiceConfig {
    std::string m_endpoint;
    std::string m_global_prefix;
    std::string m_item_prefix{"node"};
};

class HsmNodeService : public CrudService<HsmNode> {
  public:
    using Ptr = std::unique_ptr<HsmNodeService>;
    HsmNodeService(
        const HsmNodeServiceConfig& config,
        std::unique_ptr<CrudClient<HsmNode>> client);

    static Ptr create(
        const HsmNodeServiceConfig& config, KeyValueStoreClient* client);

    static Ptr create(const HsmNodeServiceConfig& config, HttpClient* client);

    virtual ~HsmNodeService();

  private:
    HsmNodeServiceConfig m_config;
};
}  // namespace hestia