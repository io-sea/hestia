#pragma once

#include <ostk/ObjectStoreError.h>

enum class HsmObjectStoreErrorCode
{
    NO_ERROR,
    ERROR,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    BASE_OBJECT_STORE_ERROR,
    MAX_ERROR
};

class HsmObjectStoreError : public ostk::RequestError<HsmObjectStoreErrorCode>
{
public:
    HsmObjectStoreError();
    HsmObjectStoreError(HsmObjectStoreErrorCode code, const std::string& message);
    HsmObjectStoreError(const ostk::ObjectStoreError& error, const std::string& message);
    
    std::string toString() const override;

private:
    std::string codeAsString() const override;
    static std::string codeToString(HsmObjectStoreErrorCode code);
    ostk::ObjectStoreError mBaseObjectStoreError;
};