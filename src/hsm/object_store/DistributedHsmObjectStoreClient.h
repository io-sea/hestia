#pragma once

#include "ErrorUtils.h"
#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientFactory.h"
#include "HttpRequest.h"

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

    OpStatus do_initialize(
        const std::string& cache_path, DistributedHsmService* hsm_service);

    void make_request(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func = nullptr,
        Stream* stream             = nullptr) const noexcept override;

  private:
    void do_remote_get(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        Stream* stream) const;

    void do_remote_put(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        Stream* stream) const;

    void do_remote_copy_or_move(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        bool is_copy) const;

    void do_remote_release(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const;

    void do_remote_op(
        const HttpRequest http_request,
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        const std::string& endpoint,
        Stream* stream = nullptr) const;

    void do_local_op(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        Stream* stream,
        const std::string& tier) const;

    HsmObjectStoreResponse::Ptr do_local_op(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        const std::string& tier) const;

    bool check_remote_config(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func) const;

    void do_local_copy_or_move(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        bool is_copy) const;

    void do_remote_copy_or_move_with_local_source(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        bool is_copy) const;

    void do_remote_copy_or_move_with_local_target(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        bool is_copy) const;

    void do_local_release(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const;

    bool is_controller_node() const;

    void get(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void put(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void copy(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void move(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void remove(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    DistributedHsmService* m_hsm_service{nullptr};
    HttpClient* m_http_client{nullptr};
    S3Client* m_s3_client{nullptr};
    std::unique_ptr<HsmObjectStoreClientManager> m_client_manager;
    DistributedHsmObjectStoreClientConfig m_config;
};
}  // namespace hestia