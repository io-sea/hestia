#pragma once

#include "EnumUtils.h"
#include "ErrorUtils.h"
#include "HestiaConfig.h"
#include "S3Client.h"
#include "Service.h"

#include <memory>

namespace hestia {

class DistributedHsmService;
class HsmService;
class KeyValueStoreClient;
class HttpClient;
class S3Client;
class HsmObjectStoreClient;
class UserService;
class EventFeed;

class CrudServiceBackend;

class IHestiaApplication {
  public:
    virtual ~IHestiaApplication() = default;

    virtual OpStatus initialize(
        const std::string& config_path = {},
        const std::string& user_token  = {},
        const Dictionary& extra_config = {},
        const std::string& server_host = {},
        unsigned server_port           = 8080) = 0;

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
        const Dictionary& extra_config = {},
        const std::string& server_host = {},
        unsigned server_port           = 8080) override;

    std::string get_runtime_info() const override;

    OpStatus run() override { return {}; }

    const std::string& get_cache_path() const;

  protected:
    virtual bool uses_local_storage() const;

    virtual void initialize_logger() const;

    virtual void set_app_mode(const std::string& host, unsigned port) = 0;

    virtual void setup_http_clients();
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

    std::unique_ptr<EventFeed> m_event_feed;
    std::unique_ptr<KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<HttpClient> m_http_client;
    std::unique_ptr<S3Client> m_s3_client;
    std::unique_ptr<HsmObjectStoreClient> m_object_store_client;
    std::unique_ptr<UserService> m_user_service;
};
}  // namespace hestia