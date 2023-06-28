#pragma once

#include "CrudService.h"
#include "Namespace.h"

#include <memory>

namespace hestia {

class KeyValueStoreClient;
class HttpClient;

struct NamespaceServiceConfig {
    std::string m_endpoint;
    std::string m_global_prefix;
    std::string m_item_prefix{"namespace"};
};

class NamespaceService : public CrudService<Namespace> {
  public:
    using Ptr = std::unique_ptr<NamespaceService>;

    NamespaceService(
        const NamespaceServiceConfig& config,
        std::unique_ptr<CrudClient<Namespace>> client);

    static Ptr create(
        const NamespaceServiceConfig& config, KeyValueStoreClient* client);

    static Ptr create(const NamespaceServiceConfig& config, HttpClient* client);

    virtual ~NamespaceService() = default;

  private:
    NamespaceServiceConfig m_config;
};
}  // namespace hestia