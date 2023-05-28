#pragma once

#include "Metadata.h"
#include "Request.h"

#include <vector>

namespace hestia {
enum class KeyValueStoreRequestMethod {
    STRING_EXISTS,
    STRING_GET,
    STRING_MULTI_GET,
    STRING_SET,
    STRING_REMOVE,
    SET_ADD,
    SET_LIST,
    SET_REMOVE
};

class KeyValueStoreRequest :
    public MethodRequest<KeyValueStoreRequestMethod>,
    public BaseRequest {
  public:
    KeyValueStoreRequest(
        KeyValueStoreRequestMethod method,
        const Metadata::Query& query,
        const std::string& url = {});

    KeyValueStoreRequest(
        KeyValueStoreRequestMethod method,
        const std::string& key,
        const std::string& url = {});

    KeyValueStoreRequest(
        KeyValueStoreRequestMethod method,
        const std::vector<std::string>& keys,
        const std::string& url = {});

    const Metadata::Query& get_query() const;

    const std::string& get_key() const;

    const std::vector<std::string>& get_keys() const;

    std::string method_as_string() const override;

  private:
    std::string m_key;
    std::vector<std::string> m_keys;
    Metadata::Query m_query;
};
}  // namespace hestia