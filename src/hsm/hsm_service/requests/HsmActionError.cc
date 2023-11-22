#include "HsmActionError.h"

namespace hestia {
HsmActionError::HsmActionError() :
    RequestError<HsmActionErrorCode>(HsmActionErrorCode::NO_ERROR, {})
{
}

HsmActionError::HsmActionError(
    HsmActionErrorCode code, const std::string& message) :
    RequestError<HsmActionErrorCode>(code, message)
{
}

std::string HsmActionError::code_as_string() const
{
    return code_to_string(m_code);
}

std::string HsmActionError::code_to_string(HsmActionErrorCode code)
{
    switch (code) {
        case HsmActionErrorCode::NO_ERROR:
            return "NO_ERROR";
        case HsmActionErrorCode::ERROR:
            return "ERROR";
        case HsmActionErrorCode::STL_EXCEPTION:
            return "STL_EXCEPTION";
        case HsmActionErrorCode::CRUD_ERROR:
            return "CRUD_ERROR";
        case HsmActionErrorCode::OBJECT_STORE_ERROR:
            return "OBJECT_STORE_ERROR";
        case HsmActionErrorCode::UNKNOWN_EXCEPTION:
            return "UNKNOWN_EXCEPTION";
        case HsmActionErrorCode::UNSUPPORTED_REQUEST_METHOD:
            return "UNSUPPORTED_REQUEST_METHOD";
        case HsmActionErrorCode::MAX_ERROR:
            return "MAX_ERROR";
        case HsmActionErrorCode::ITEM_NOT_FOUND:
            return "ITEM_NOT_FOUND";
        case HsmActionErrorCode::BAD_PUT_OVERWRITE_COMBINATION:
            return "MAX_ERROR";
        default:
            return "UNKOWN ERROR";
    }
}
}  // namespace hestia