#include "HsmServiceError.h"

namespace hestia {
HsmServiceError::HsmServiceError() :
    RequestError<HsmServiceErrorCode>(HsmServiceErrorCode::NO_ERROR, {})
{
}

HsmServiceError::HsmServiceError(
    HsmServiceErrorCode code, const std::string& message) :
    RequestError<HsmServiceErrorCode>(code, message)
{
}

std::string HsmServiceError::code_as_string() const
{
    return code_to_string(m_code);
}

std::string HsmServiceError::code_to_string(HsmServiceErrorCode code)
{
    switch (code) {
        case HsmServiceErrorCode::NO_ERROR:
            return "NO_ERROR";
        case HsmServiceErrorCode::ERROR:
            return "ERROR";
        case HsmServiceErrorCode::STL_EXCEPTION:
            return "STL_EXCEPTION";
        case HsmServiceErrorCode::UNKNOWN_EXCEPTION:
            return "UNKNOWN_EXCEPTION";
        case HsmServiceErrorCode::UNSUPPORTED_REQUEST_METHOD:
            return "UNSUPPORTED_REQUEST_METHOD";
        case HsmServiceErrorCode::MAX_ERROR:
            return "MAX_ERROR";
        case HsmServiceErrorCode::OBJECT_NOT_FOUND:
            return "BAD_PUT_OVERWRITE_COMBINATION";
        case HsmServiceErrorCode::BAD_PUT_OVERWRITE_COMBINATION:
            return "MAX_ERROR";
        default:
            return "UNKOWN ERROR";
    }
}
}  // namespace hestia