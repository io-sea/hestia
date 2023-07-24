#pragma once

#include "Request.h"

#include <string>
#include <vector>

namespace hestia {
enum class KeyValueStoreRequestMethod {
    STRING_EXISTS,
    STRING_GET,
    STRING_SET,
    STRING_REMOVE,
    SET_ADD,
    SET_LIST,
    SET_REMOVE
};

using KeyValuePair    = std::pair<std::string, std::string>;
using VecKeyValuePair = std::vector<KeyValuePair>;

class KeyValueStoreRequest :
    public MethodRequest<KeyValueStoreRequestMethod>,
    public BaseRequest {
  public:
    KeyValueStoreRequest(
        KeyValueStoreRequestMethod method,
        const VecKeyValuePair& kv_pairs,
        const std::string& url = {});

    KeyValueStoreRequest(
        KeyValueStoreRequestMethod method,
        const std::vector<std::string>& keys,
        const std::string& url = {});

    const VecKeyValuePair& get_kv_pairs() const;

    const std::vector<std::string>& get_keys() const;

    std::string method_as_string() const override;

  private:
    std::vector<std::string> m_keys;
    VecKeyValuePair m_kv_pairs;
};
}  // namespace hestia