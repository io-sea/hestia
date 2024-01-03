#pragma once

#include "HsmObjectStoreContext.h"
#include "ObjectStoreClient.h"

#include <vector>

namespace hestia {
class HsmObjectStoreClient : public ObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClient>;

    virtual ~HsmObjectStoreClient();

    using completionFunc = std::function<void(HsmObjectStoreResponse::Ptr)>;
    using progressFunc   = std::function<void(HsmObjectStoreResponse::Ptr)>;

    virtual void make_request(HsmObjectStoreContext& ctx) const noexcept;

    virtual void set_tier_ids(const std::vector<std::string>& tier_ids);

  protected:
    void make_request(ObjectStoreContext& ctx) const noexcept override;

    virtual void get(HsmObjectStoreContext& ctx) const = 0;

    virtual void put(HsmObjectStoreContext& ctx) const = 0;

    virtual void copy(HsmObjectStoreContext& ctx) const = 0;

    virtual void move(HsmObjectStoreContext& ctx) const = 0;

    virtual void remove(HsmObjectStoreContext& ctx) const = 0;

    // hestia::ObjectStoreClient Methods
    bool exists(const StorageObject& object) const override;

    void get(ObjectStoreContext& ctx) const override;

    void put(ObjectStoreContext& ctx) const override;

    void remove(const StorageObject& object) const override;

    void list(const KeyValuePair& query, std::vector<StorageObject>& fetched)
        const override;

    void on_success(const HsmObjectStoreContext& ctx) const;

    std::size_t get_tier_index(const std::string& id) const;

    std::vector<std::string> m_tier_ids;

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