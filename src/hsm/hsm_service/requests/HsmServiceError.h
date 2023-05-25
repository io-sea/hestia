#pragma once

#include "RequestError.h"

namespace hestia {
enum class HsmServiceErrorCode {
    NO_ERROR,
    ERROR,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    OBJECT_NOT_FOUND,
    BAD_PUT_OVERWRITE_COMBINATION,
    MAX_ERROR
};

class HsmServiceError : public RequestError<HsmServiceErrorCode> {
  public:
    HsmServiceError();
    HsmServiceError(HsmServiceErrorCode code, const std::string& message);
    virtual ~HsmServiceError() = default;

  private:
    std::string code_as_string() const override;
    static std::string code_to_string(HsmServiceErrorCode code);
};
}  // namespace hestia