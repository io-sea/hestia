#pragma once

#include "CrudServiceBackend.h"
#include "CrudServiceWithUser.h"
#include "HsmActionResponse.h"
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

    [[nodiscard]] CrudResponse::Ptr make_request(
        const CrudRequest& request,
        const std::string& type = {}) const noexcept;

    using actionCompletionFunc = std::function<void(HsmActionResponse::Ptr)>;
    using actionProgressFunc   = std::function<void(HsmActionResponse::Ptr)>;
    void do_hsm_action(
        const HsmActionRequest& request,
        Stream* stream,
        actionCompletionFunc completion_func,
        actionProgressFunc progress_func = nullptr) const noexcept;

    HsmService* get_hsm_service();

    UserService* get_user_service();

    const std::vector<ObjectStoreBackend>& get_backends() const;

    const DistributedHsmServiceConfig& get_self_config() const;

    bool is_server() const;

    const std::string& get_controller_address() const;

    std::string get_backend_address(
        const std::string& tier_id, const std::string& preferred_address) const;

    void register_self();

  private:
    void register_backends();

    std::pair<std::string, unsigned> get_node_address(
        const std::string& node_id) const;

    DistributedHsmServiceConfig m_config;
    HsmServicePtr m_hsm_service;
    UserService* m_user_service;
};

}  // namespace hestia
