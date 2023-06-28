#pragma once

#include "DistributedHsmServiceRequest.h"
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
    std::string m_app_name;
    bool m_is_server{true};
};

class DistributedHsmService {
  public:
    using Ptr = std::unique_ptr<DistributedHsmService>;
    DistributedHsmService(
        DistributedHsmServiceConfig config,
        std::unique_ptr<HsmService> hsm_service,
        std::unique_ptr<HsmNodeService> node_service);

    static Ptr create(
        DistributedHsmServiceConfig config,
        std::unique_ptr<HsmService> hsm_service,
        KeyValueStoreClient* client);
    static Ptr create(
        DistributedHsmServiceConfig config,
        std::unique_ptr<HsmService> hsm_service,
        HttpClient* client);

    ~DistributedHsmService();

    HsmService* get_hsm_service();

    const DistributedHsmServiceConfig& get_self_config() const;

    DistributedHsmServiceResponse::Ptr make_request(
        const DistributedHsmServiceRequest& req) const noexcept;

  private:
    void register_self();

    DistributedHsmServiceResponse::Ptr get(
        const DistributedHsmServiceRequest& req) const;

    DistributedHsmServiceResponse::Ptr put(
        const DistributedHsmServiceRequest& req) const;

    DistributedHsmServiceResponse::Ptr list(
        const DistributedHsmServiceRequest& req) const;

    DistributedHsmServiceConfig m_config;
    std::unique_ptr<HsmService> m_hsm_service;
    std::unique_ptr<HsmNodeService> m_node_service;
};

}  // namespace hestia
