#pragma once

#include "EnumUtils.h"
#include "ErrorUtils.h"
#include "HestiaConfig.h"

#include <memory>

namespace hestia {

class DistributedHsmService;
class HsmService;
class KeyValueStoreClient;
class HttpClient;
class HsmObjectStoreClient;
class UserService;

class CrudServiceBackend;

class IHestiaApplication {
  public:
    virtual ~IHestiaApplication() = default;

    virtual OpStatus initialize(
        const std::string& config_path = {},
        const std::string& user_token  = {},
        const Dictionary& extra_config = {}) = 0;

    virtual std::string get_runtime_info() const = 0;

    virtual OpStatus run() = 0;
};

class HestiaApplication : public IHestiaApplication {
  public:
    STRINGABLE_ENUM(
        ApplicationMode,
        CLIENT_STANDALONE,
        CLIENT_FULL,
        SERVER_CONTROLLER,
        SERVER_WORKER)

    HestiaApplication() = default;

    virtual ~HestiaApplication();

    OpStatus initialize(
        const std::string& config_path = {},
        const std::string& user_token  = {},
        const Dictionary& extra_config = {}) override;

    std::string get_runtime_info() const override;

    OpStatus run() override { return {}; }

  protected:
    virtual bool uses_local_storage() const;
    virtual bool uses_http_client() const;

    virtual void initialize_logger() const;

    virtual void set_app_mode() = 0;

    virtual void setup_http_client();
    virtual void setup_key_value_store();

    virtual void setup_object_store();

    virtual void setup_user_service(
        const ServiceConfig& config, CrudServiceBackend* backend);
    virtual void setup_hsm_service(
        const ServiceConfig& config, CrudServiceBackend* backend);

    HestiaConfig m_config;
    ApplicationMode m_app_mode{ApplicationMode::CLIENT_STANDALONE};

    std::unique_ptr<DistributedHsmService> m_distributed_hsm_service;
    HsmService* m_hsm_service{nullptr};

    std::unique_ptr<KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<HttpClient> m_http_client;
    std::unique_ptr<HsmObjectStoreClient> m_object_store_client;
    std::unique_ptr<UserService> m_user_service;
};
}  // namespace hestia