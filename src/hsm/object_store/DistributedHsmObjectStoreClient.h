#pragma once

#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientFactory.h"

#include <memory>

namespace hestia {

class HttpClient;
class S3Client;
class HsmObjectStoreClientManager;
class DistributedHsmService;

class DistributedHsmObjectStoreClientConfig {
  public:
    std::size_t m_buffer_size{4096};
};

class DistributedHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<DistributedHsmObjectStoreClient>;

    DistributedHsmObjectStoreClient(
        std::unique_ptr<HsmObjectStoreClientManager> client_manager,
        HttpClient* http_client                      = nullptr,
        S3Client* s3_client                          = nullptr,
        DistributedHsmObjectStoreClientConfig config = {});

    static Ptr create(
        HttpClient* http_client                                = nullptr,
        S3Client* s3_client                                    = nullptr,
        const std::vector<std::filesystem::path>& plugin_paths = {});

    virtual ~DistributedHsmObjectStoreClient();

    void do_initialize(
        const std::string& cache_path, DistributedHsmService* hsm_service);

    [[nodiscard]] HsmObjectStoreResponse::Ptr make_request(
        const HsmObjectStoreRequest& request,
        Stream* stream = nullptr) const noexcept override;

  private:
    HsmObjectStoreResponse::Ptr do_remote_get(
        const HsmObjectStoreRequest& request, Stream* stream) const;

    HsmObjectStoreResponse::Ptr do_remote_put(
        const HsmObjectStoreRequest& request, Stream* stream) const;

    HsmObjectStoreResponse::Ptr do_remote_copy_or_move(
        const HsmObjectStoreRequest& request, bool is_copy) const;

    HsmObjectStoreResponse::Ptr do_remote_release(
        const HsmObjectStoreRequest& request) const;

    HsmObjectStoreResponse::Ptr do_local_op(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        uint8_t tier) const;

    HsmObjectStoreResponse::Ptr do_local_copy_or_move(
        const HsmObjectStoreRequest& request, bool is_copy) const;

    HsmObjectStoreResponse::Ptr do_remote_copy_or_move_with_local_source(
        const HsmObjectStoreRequest& request, bool is_copy) const;

    HsmObjectStoreResponse::Ptr do_remote_copy_or_move_with_local_target(
        const HsmObjectStoreRequest& request, bool is_copy) const;

    bool is_controller_node() const;

    void copy(const HsmObjectStoreRequest& request) const override
    {
        (void)request;
    };

    void get(
        const HsmObjectStoreRequest& request,
        StorageObject& object,
        Stream* stream) const override
    {
        (void)request;
        (void)object, (void)stream;
    };

    void move(const HsmObjectStoreRequest& request) const override
    {
        (void)request;
    };

    void put(
        const HsmObjectStoreRequest& request, Stream* stream) const override
    {
        (void)request;
        (void)stream;
    };

    void remove(const HsmObjectStoreRequest& request) const override
    {
        (void)request;
    };

    DistributedHsmService* m_hsm_service{nullptr};
    HttpClient* m_http_client{nullptr};
    S3Client* m_s3_client{nullptr};
    std::unique_ptr<HsmObjectStoreClientManager> m_client_manager;
    DistributedHsmObjectStoreClientConfig m_config;
};
}  // namespace hestia