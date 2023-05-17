#include "KeyValueStoreError.h"

namespace hestia {

KeyValueStoreError::KeyValueStoreError() :
    RequestError<KeyValueStoreErrorCode>(KeyValueStoreErrorCode::NO_ERROR, {})
{
}

KeyValueStoreError::KeyValueStoreError(
    KeyValueStoreErrorCode code, const std::string& message) :
    RequestError<KeyValueStoreErrorCode>(code, message)
{
}

std::string KeyValueStoreError::code_as_string() const
{
    return code_to_string(m_code);
}

std::string KeyValueStoreError::code_to_string(KeyValueStoreErrorCode code)
{
    switch (code) {
        case KeyValueStoreErrorCode::NO_ERROR:
            return "NO_ERROR";
        case KeyValueStoreErrorCode::ERROR:
            return "ERROR";
        case KeyValueStoreErrorCode::UNSUPPORTED_REQUEST_METHOD:
            return "UNSUPPORTED_REQUEST_METHOD";
        case KeyValueStoreErrorCode::STL_EXCEPTION:
            return "STL_EXCEPTION";
        case KeyValueStoreErrorCode::UNKNOWN_EXCEPTION:
            return "UNKNOWN_EXCEPTION";
        case KeyValueStoreErrorCode::MAX_ERROR:
            return "MAX_ERROR";
        default:
            return "UNKOWN ERROR";
    }
}
}  // namespace hestia