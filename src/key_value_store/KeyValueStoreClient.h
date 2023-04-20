#pragma once

#include <ostk/ObjectStoreResponse.h>

#include <vector>

class KeyValueStoreClient {
  public:
    virtual ~KeyValueStoreClient() = default;

    [[nodiscard]] ostk::ObjectStoreResponse::Ptr makeRequest(
        const ostk::ObjectStoreRequest& request) const noexcept;

  private:
    virtual bool exists(const ostk::StorageObject& obj) const = 0;

    virtual void get(
        ostk::StorageObject& obj,
        const std::vector<std::string>& keys = {}) const = 0;

    virtual void put(
        const ostk::StorageObject& obj,
        const std::vector<std::string>& keys = {}) const = 0;

    virtual void remove(const ostk::StorageObject& obj) const = 0;

    virtual void list(
        const ostk::Metadata::Query& query,
        std::vector<ostk::StorageObject>& fetched) const;
};