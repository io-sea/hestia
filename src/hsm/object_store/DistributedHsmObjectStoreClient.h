#pragma once

#include "HsmObjectStoreClient.h"
#include "ObjectStoreClientFactory.h"

#include "ErrorUtils.h"
#include "HttpRequest.h"

#include <memory>

namespace hestia {

class HttpClient;
class S3Client;
class ObjectStoreClientManager;
class DistributedHsmService;
class StorageTier;

class DistributedHsmObjectStoreClientConfig {
  public:
    std::size_t m_buffer_size{4096};
};

class DistributedHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<DistributedHsmObjectStoreClient>;

    DistributedHsmObjectStoreClient(
        std::unique_ptr<ObjectStoreClientManager> client_manager,
        HttpClient* http_client                      = nullptr,
        S3Client* s3_client                          = nullptr,
        DistributedHsmObjectStoreClientConfig config = {});

    static Ptr create(
        HttpClient* http_client                                = nullptr,
        S3Client* s3_client                                    = nullptr,
        const std::vector<std::filesystem::path>& plugin_paths = {});

    virtual ~DistributedHsmObjectStoreClient();

    OpStatus do_initialize(
        const std::string& cache_path, DistributedHsmService* hsm_service);

    void make_request(HsmObjectStoreContext& ctx) const noexcept override;

  private:
    void make_single_tier_request(HsmObjectStoreContext& ctx) const noexcept;

    void make_multi_tier_request(HsmObjectStoreContext& ctx) const noexcept;

    void do_local_op(HsmObjectStoreContext& ctx) const;

    void do_local_hsm(HsmObjectStoreContext& ctx) const;

    void do_remote_op(
        HsmObjectStoreContext& ctx, const std::string& endpoint = {}) const;

    void do_remote_op(
        const HttpRequest http_request, HsmObjectStoreContext& ctx) const;

    void do_remote_hsm_with_local_source(HsmObjectStoreContext& ctx) const;

    void do_remote_hsm_with_local_target(HsmObjectStoreContext& ctx) const;

    void on_remote_response(
        HttpResponse::Ptr http_response, HsmObjectStoreContext& ctx) const;

    void on_error(
        const HsmObjectStoreContext& ctx,
        HsmObjectStoreErrorCode error_code,
        const std::string& msg) const;

    bool is_controller_node() const;

    bool check_remote_config(HsmObjectStoreContext& ctx) const;

    void get(HsmObjectStoreContext& ctx) const override;

    void put(HsmObjectStoreContext& ctx) const override;

    void copy(HsmObjectStoreContext& ctx) const override;

    void move(HsmObjectStoreContext& ctx) const override;

    void remove(HsmObjectStoreContext& ctx) const override;

    OpStatus read_tiers(std::vector<StorageTier>& tiers) const;

    std::string get_action_path() const;

    DistributedHsmService* m_hsm_service{nullptr};
    HttpClient* m_http_client{nullptr};
    S3Client* m_s3_client{nullptr};
    std::unique_ptr<ObjectStoreClientManager> m_client_manager;
    DistributedHsmObjectStoreClientConfig m_config;
};
}  // namespace hestia