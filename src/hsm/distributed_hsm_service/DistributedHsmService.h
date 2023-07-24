#pragma once

#include "CrudServiceBackend.h"
#include "CrudServiceWithUser.h"
#include "HsmNode.h"

#include <vector>

namespace hestia {
class KeyValueStoreClient;
class HttpClient;
class UserService;

class HsmService;
using HsmServicePtr = std::unique_ptr<HsmService>;

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
        HsmServicePtr hsm_service,
        CrudService::Ptr node_service);

    static Ptr create(
        DistributedHsmServiceConfig config,
        HsmServicePtr hsm_service,
        CrudServiceBackend* backend,
        UserService* user_service);

    ~DistributedHsmService();

    HsmService* get_hsm_service();

    CrudService* get_node_service();

    const DistributedHsmServiceConfig& get_self_config() const;

    void register_self();

  private:
    DistributedHsmServiceConfig m_config;
    HsmServicePtr m_hsm_service;
    CrudService::Ptr m_node_service;
};

}  // namespace hestia
