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
    std::vector<ObjectStoreBackend> m_backends;
    bool m_is_server{false};
};

class DistributedHsmService {
  public:
    using Ptr = std::unique_ptr<DistributedHsmService>;
    DistributedHsmService(
        DistributedHsmServiceConfig config,
        HsmServicePtr hsm_service,
        UserService* user_service);

    static Ptr create(
        DistributedHsmServiceConfig config,
        HsmServicePtr hsm_service,
        UserService* user_service);

    ~DistributedHsmService();

    HsmService* get_hsm_service();

    UserService* get_user_service();

    const std::vector<ObjectStoreBackend>& get_backends() const;

    const DistributedHsmServiceConfig& get_self_config() const;

    void register_self();

  private:
    void register_backends();

    DistributedHsmServiceConfig m_config;
    HsmServicePtr m_hsm_service;
    UserService* m_user_service;
};

}  // namespace hestia
