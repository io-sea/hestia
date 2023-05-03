#include "HsmObjectStoreError.h"

namespace hestia {
HsmObjectStoreError::HsmObjectStoreError() :
    RequestError<HsmObjectStoreErrorCode>(HsmObjectStoreErrorCode::NO_ERROR, {})
{
}

HsmObjectStoreError::HsmObjectStoreError(
    HsmObjectStoreErrorCode code, const std::string& message) :
    RequestError<HsmObjectStoreErrorCode>(code, message)
{
}

HsmObjectStoreError::HsmObjectStoreError(
    const ObjectStoreError& error, const std::string& message) :
    RequestError<HsmObjectStoreErrorCode>(
        HsmObjectStoreErrorCode::BASE_OBJECT_STORE_ERROR, message),
    m_base_object_store_error(error)
{
}

std::string HsmObjectStoreError::to_string() const
{
    std::string msg = "# " + std::to_string(number()) + " | "
                      + code_to_string(m_code) + " | " + m_message;
    if (m_code == HsmObjectStoreErrorCode::BASE_OBJECT_STORE_ERROR) {
        msg += "\n" + m_base_object_store_error.to_string();
    }
    return msg;
}

std::string HsmObjectStoreError::code_as_string() const
{
    return code_to_string(m_code);
}

std::string HsmObjectStoreError::code_to_string(HsmObjectStoreErrorCode code)
{
    switch (code) {
        case HsmObjectStoreErrorCode::NO_ERROR:
            return "NO_ERROR";
        case HsmObjectStoreErrorCode::ERROR:
            return "ERROR";
        case HsmObjectStoreErrorCode::STL_EXCEPTION:
            return "STL_EXCEPTION";
        case HsmObjectStoreErrorCode::UNKNOWN_EXCEPTION:
            return "UNKNOWN_EXCEPTION";
        case HsmObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD:
            return "UNSUPPORTED_REQUEST_METHOD";
        case HsmObjectStoreErrorCode::MAX_ERROR:
            return "MAX_ERROR";
        default:
            return "UNKOWN ERROR";
    }
}
}  // namespace hestia