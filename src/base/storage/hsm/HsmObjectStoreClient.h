#pragma once

#include "HsmObjectStoreRequest.h"
#include "HsmObjectStoreResponse.h"

#include "ObjectStoreClient.h"

#include <vector>

namespace hestia {
class HsmObjectStoreClient : public ObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClient>;

    virtual ~HsmObjectStoreClient();

    using completionFunc = std::function<void(HsmObjectStoreResponse::Ptr)>;
    using progressFunc   = std::function<void(HsmObjectStoreResponse::Ptr)>;

    virtual void make_request(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func = nullptr,
        Stream* stream             = nullptr) const noexcept;

    void set_tier_names(const std::vector<std::string>& tier_names);

  protected:
    void make_request(
        const ObjectStoreRequest& request,
        ObjectStoreClient::completionFunc completion_func,
        ObjectStoreClient::progressFunc progress_func = nullptr,
        Stream* stream = nullptr) const noexcept override;

    virtual void get(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    virtual void put(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    virtual void copy(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    virtual void move(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    virtual void remove(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const = 0;

    // hestia::ObjectStoreClient Methods
    bool exists(const StorageObject& object) const override;

    void get(
        const ObjectStoreRequest& request,
        ObjectStoreClient::completionFunc completion_func,
        Stream* stream                     = nullptr,
        Stream::progressFunc progress_func = nullptr) const override;

    void put(
        const ObjectStoreRequest& request,
        ObjectStoreClient::completionFunc completion_func,
        Stream* stream                     = nullptr,
        Stream::progressFunc progress_func = nullptr) const override;

    void remove(const StorageObject& object) const override;

    void list(const KeyValuePair& query, std::vector<StorageObject>& fetched)
        const override;

    std::vector<std::string> m_tier_names;

  private:
    void on_exception(
        const HsmObjectStoreRequest& request,
        HsmObjectStoreResponse* response,
        const std::string& message = {}) const;

    void on_exception(
        const HsmObjectStoreRequest& request,
        HsmObjectStoreResponse* response,
        const hestia::ObjectStoreError& error) const;

    void on_exception(
        const HsmObjectStoreRequest& request,
        HsmObjectStoreResponse* response,
        const HsmObjectStoreError& error) const;
};
}  // namespace hestia