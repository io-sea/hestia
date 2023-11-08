#pragma once

#include "RequestError.h"
#include "RequestException.h"

#include <ostream>
#include <string>

namespace hestia {

enum class ObjectStoreErrorCode {
    NO_ERROR,
    ERROR,
    STL_EXCEPTION,
    BAD_STREAM,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    OBJECT_NOT_FOUND,
    MAX_ERROR
};

class ObjectStoreError : public RequestError<ObjectStoreErrorCode> {
  public:
    ObjectStoreError();
    ObjectStoreError(ObjectStoreErrorCode code, const std::string& message);
    virtual ~ObjectStoreError() = default;

  private:
    std::string code_as_string() const override;
    static std::string code_to_string(ObjectStoreErrorCode code);
};

using ObjectStoreException = RequestException<ObjectStoreError>;

}  // namespace hestia