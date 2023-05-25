#pragma once

#include "HestiaNode.h"
#include "KeyValueCrudService.h"

#include <memory>

namespace hestia {
class KeyValueStoreClient;
class HttpClient;

struct HestiaNodeServiceConfig {
    std::string m_endpoint;
};

class HestiaNodeService : public KeyValueCrudService<HestiaNode> {
  public:
    using Ptr = std::unique_ptr<HestiaNodeService>;
    HestiaNodeService(
        const HestiaNodeServiceConfig& config,
        KeyValueStoreClient* kv_store_client,
        HttpClient* http_client = nullptr);

    virtual ~HestiaNodeService();

    void to_string(const HestiaNode& item, std::string& output) const override;

    void from_string(
        const std::string& output, HestiaNode& item) const override;

    HttpClient* m_http_client{nullptr};
};
}  // namespace hestia