#pragma once

#include "KeyValueStoreRequest.h"
#include "Metadata.h"
#include "Response.h"
#include "Uuid.h"

#include <memory>
#include <vector>

namespace hestia {

using KeyValueStoreResponse    = CrudResponse<std::string, CrudErrorCode>;
using KeyValueStoreResponsePtr = std::unique_ptr<KeyValueStoreResponse>;

class KeyValueStoreClient {
  public:
    virtual ~KeyValueStoreClient();

    virtual void initialize(const Metadata& config) { (void)config; };

    [[nodiscard]] KeyValueStoreResponsePtr make_request(
        const KeyValueStoreRequest& request) const noexcept;

  protected:
    virtual void string_get(
        const std::string& key, std::string& value) const = 0;

    virtual void string_multi_get(
        const std::vector<std::string>& key,
        std::vector<std::string>& value) const = 0;

    virtual bool string_exists(const std::string& key) const = 0;

    virtual void string_set(
        const std::string& key, const std::string& value) const = 0;

    virtual void string_remove(const std::string& key) const = 0;

    virtual void set_add(const std::string& key, const Uuid& value) const = 0;

    virtual void set_list(
        const std::string& key, std::vector<Uuid>& values) const = 0;

    virtual void set_remove(
        const std::string& key, const Uuid& value) const = 0;

    void on_exception(
        const KeyValueStoreRequest& request,
        KeyValueStoreResponse* response,
        const std::string& message = {}) const;

    void on_exception(
        const KeyValueStoreRequest& request,
        KeyValueStoreResponse* response,
        const RequestError<CrudErrorCode>& error) const;
};
}  // namespace hestia