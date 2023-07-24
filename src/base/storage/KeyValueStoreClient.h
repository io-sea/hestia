#pragma once

#include "Dictionary.h"
#include "KeyValueStoreRequest.h"
#include "KeyValueStoreResponse.h"

#include <vector>

namespace hestia {

class KeyValueStoreClient {
  public:
    virtual ~KeyValueStoreClient();

    virtual void initialize(
        const std::string& cache_path, const Dictionary& config);

    [[nodiscard]] KeyValueStoreResponse::Ptr make_request(
        const KeyValueStoreRequest& request) const noexcept;

  protected:
    virtual void string_get(
        const std::vector<std::string>& key,
        std::vector<std::string>& value) const = 0;

    virtual void string_exists(
        const std::vector<std::string>& key,
        std::vector<bool>& found) const = 0;

    virtual void string_set(const VecKeyValuePair& kv_pairs) const = 0;

    virtual void string_remove(const std::vector<std::string>& key) const = 0;

    virtual void set_add(const VecKeyValuePair& entry) const = 0;

    virtual void set_list(
        const std::vector<std::string>& key,
        std::vector<std::vector<std::string>>& values) const = 0;

    virtual void set_remove(const VecKeyValuePair& entry) const = 0;

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