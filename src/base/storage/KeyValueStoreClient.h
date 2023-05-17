#pragma once

#include "KeyValueStoreResponse.h"
#include "Metadata.h"

#include <vector>

namespace hestia {
class KeyValueStoreClient {
  public:
    virtual ~KeyValueStoreClient();

    virtual void initialize(const Metadata& config) { (void)config; };

    [[nodiscard]] KeyValueStoreResponse::Ptr make_request(
        const KeyValueStoreRequest& request) const noexcept;

  private:
    virtual void string_get(
        const std::string& key, std::string& value) const = 0;

    virtual bool string_exists(const std::string& key) const = 0;

    virtual void string_set(
        const std::string& key, const std::string& value) const = 0;

    virtual void string_remove(const std::string& key) const = 0;

    virtual void set_add(
        const std::string& key, const std::string& value) const = 0;

    virtual void set_list(
        const std::string& key, std::vector<std::string>& values) const = 0;

    virtual void set_remove(
        const std::string& key, const std::string& value) const = 0;

    void on_exception(
        const KeyValueStoreRequest& request,
        KeyValueStoreResponse* response,
        const std::string& message = {}) const;

    void on_exception(
        const KeyValueStoreRequest& request,
        KeyValueStoreResponse* response,
        const KeyValueStoreError& error) const;
};
}  // namespace hestia