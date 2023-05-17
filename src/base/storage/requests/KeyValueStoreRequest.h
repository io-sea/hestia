#pragma once

#include "Metadata.h"
#include "Request.h"

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

class KeyValueStoreRequest :
    public MethodRequest<KeyValueStoreRequestMethod>,
    public BaseRequest {
  public:
    KeyValueStoreRequest(
        KeyValueStoreRequestMethod method, const Metadata::Query& query);

    const Metadata::Query& get_query() const;

    std::string method_as_string() const override;

  private:
    Metadata::Query m_query;
};
};  // namespace hestia