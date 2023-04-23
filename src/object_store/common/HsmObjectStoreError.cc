#include "HsmObjectStoreError.h"

HsmObjectStoreError::HsmObjectStoreError() :
    ostk::RequestError<HsmObjectStoreErrorCode>(
        HsmObjectStoreErrorCode::NO_ERROR, {})
{
}

HsmObjectStoreError::HsmObjectStoreError(
    HsmObjectStoreErrorCode code, const std::string& message) :
    ostk::RequestError<HsmObjectStoreErrorCode>(code, message)
{
}

HsmObjectStoreError::HsmObjectStoreError(
    const ostk::ObjectStoreError& error, const std::string& message) :
    ostk::RequestError<HsmObjectStoreErrorCode>(
        HsmObjectStoreErrorCode::BASE_OBJECT_STORE_ERROR, message),
    m_base_object_store_error(error)
{
}

std::string HsmObjectStoreError::toString() const
{
    std::string msg = "# " + std::to_string(number()) + " | "
                      + code_to_string(mCode) + " | " + mMessage;
    if (mCode == HsmObjectStoreErrorCode::BASE_OBJECT_STORE_ERROR) {
        msg += "\n" + m_base_object_store_error.toString();
    }
    return msg;
}

std::string HsmObjectStoreError::codeAsString() const
{
    return code_to_string(mCode);
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