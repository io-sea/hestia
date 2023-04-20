#pragma once

#include "HsmObjectStoreRequest.h"
#include "HsmObjectStoreResponse.h"

#include <ostk/ObjectStoreClient.h>

class HsmObjectStoreClient : public ostk::ObjectStoreClient
{
public:
    using Ptr = std::unique_ptr<HsmObjectStoreClient>;

    [[nodiscard]] virtual HsmObjectStoreResponse::Ptr makeRequest(const HsmObjectStoreRequest& request, ostk::Stream* stream = nullptr) const noexcept;

protected:
    [[nodiscard]] ostk::ObjectStoreResponse::Ptr makeRequest(const ostk::ObjectStoreRequest& request, ostk::Stream* stream = nullptr) const noexcept override;

    virtual void copy(const HsmObjectStoreRequest& request) const = 0;

    virtual void get(const HsmObjectStoreRequest& request, ostk::StorageObject& object, ostk::Stream* stream) const = 0;

    virtual void move(const HsmObjectStoreRequest& request) const = 0;

    virtual void put(const HsmObjectStoreRequest& request, ostk::Stream* stream) const = 0;

    virtual void remove(const HsmObjectStoreRequest& request) const = 0;

    // ostk::ObjectStoreClient Methods
    bool exists(const ostk::StorageObject& object) const override;

    void get(ostk::StorageObject& object, const ostk::Extent& extent, ostk::Stream* stream) const override;

    void put(const ostk::StorageObject& object, const ostk::Extent& extent, ostk::Stream* stream) const override;

    void remove(const ostk::StorageObject& object) const override;

    void list(const ostk::Metadata::Query& query, std::vector<ostk::StorageObject>& fetched) const override;

private:
    void onException(const HsmObjectStoreRequest& request, HsmObjectStoreResponse* response, const std::string& message = {}) const;

    void onException(const HsmObjectStoreRequest& request, HsmObjectStoreResponse* response, const ostk::ObjectStoreError& error) const;

    void onException(const HsmObjectStoreRequest& request, HsmObjectStoreResponse* response, const HsmObjectStoreError& error) const;
};