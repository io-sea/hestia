#include "ObjectStoreError.h"

namespace hestia {

ObjectStoreError::ObjectStoreError() :
    RequestError<ObjectStoreErrorCode>(ObjectStoreErrorCode::NO_ERROR, {})
{
}

ObjectStoreError::ObjectStoreError(
    ObjectStoreErrorCode code, const std::string& message) :
    RequestError<ObjectStoreErrorCode>(code, message)
{
}

std::string ObjectStoreError::code_as_string() const
{
    return code_to_string(m_code);
}

std::string ObjectStoreError::code_to_string(ObjectStoreErrorCode code)
{
    switch (code) {
        case ObjectStoreErrorCode::NO_ERROR:
            return "NO_ERROR";
        case ObjectStoreErrorCode::ERROR:
            return "ERROR";
        case ObjectStoreErrorCode::STL_EXCEPTION:
            return "STL_EXCEPTION";
        case ObjectStoreErrorCode::BAD_STREAM:
            return "BAD_STREAM";
        case ObjectStoreErrorCode::UNKNOWN_EXCEPTION:
            return "UNKNOWN_EXCEPTION";
        case ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD:
            return "UNSUPPORTED_REQUEST_METHOD";
        case ObjectStoreErrorCode::MAX_ERROR:
            return "MAX_ERROR";
        case ObjectStoreErrorCode::OBJECT_NOT_FOUND:
            return "OBJECT_NOT_FOUND";
        default:
            return "UNKOWN ERROR";
    }
}
}  // namespace hestia