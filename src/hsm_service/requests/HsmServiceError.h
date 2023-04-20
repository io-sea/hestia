#pragma once

#include <ostk/RequestError.h>

enum class HsmServiceErrorCode
{
    NO_ERROR,
    ERROR,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    OBJECT_NOT_FOUND,
    BAD_PUT_OVERWRITE_COMBINATION,
    MAX_ERROR
};

class HsmServiceError : public ostk::RequestError<HsmServiceErrorCode>
{
public:
    HsmServiceError();
    HsmServiceError(HsmServiceErrorCode code, const std::string& message);
    virtual ~HsmServiceError() = default;

private:
    std::string codeAsString() const override;
    static std::string codeToString(HsmServiceErrorCode code);
};