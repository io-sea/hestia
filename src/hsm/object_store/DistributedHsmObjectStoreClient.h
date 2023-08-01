#pragma once

#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientFactory.h"

#include <memory>

namespace hestia {

class HttpClient;
class HsmObjectStoreClientManager;
class DistributedHsmService;

class DistributedHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<DistributedHsmObjectStoreClient>;

    DistributedHsmObjectStoreClient(
        std::unique_ptr<HsmObjectStoreClientManager> client_manager,
        HttpClient* http_client = nullptr);

    static Ptr create(
        HttpClient* http_client                                = nullptr,
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

    HsmObjectStoreResponse::Ptr do_local_op(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        uint8_t tier) const;

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
    std::unique_ptr<HsmObjectStoreClientManager> m_client_manager;
};
}  // namespace hestia