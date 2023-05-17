#pragma once

#include "RequestError.h"
#include "RequestException.h"

#include <ostream>
#include <string>

namespace hestia {
enum class KeyValueStoreErrorCode {
    NO_ERROR,
    ERROR,
    UNSUPPORTED_REQUEST_METHOD,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    MAX_ERROR
};

class KeyValueStoreError : public RequestError<KeyValueStoreErrorCode> {
  public:
    KeyValueStoreError();
    KeyValueStoreError(KeyValueStoreErrorCode code, const std::string& message);
    virtual ~KeyValueStoreError() = default;

  private:
    std::string code_as_string() const override;
    static std::string code_to_string(KeyValueStoreErrorCode code);
};

using KeyValueStoreException = RequestException<KeyValueStoreError>;
}  // namespace hestia