#pragma once

#include "RequestError.h"

namespace hestia {
enum class HsmActionErrorCode {
    NO_ERROR,
    ERROR,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    ITEM_NOT_FOUND,
    BAD_PUT_OVERWRITE_COMBINATION,
    MAX_ERROR
};

class HsmActionError : public RequestError<HsmActionErrorCode> {
  public:
    HsmActionError();
    HsmActionError(HsmActionErrorCode code, const std::string& message);
    virtual ~HsmActionError() = default;

  private:
    std::string code_as_string() const override;
    static std::string code_to_string(HsmActionErrorCode code);
};
}  // namespace hestia