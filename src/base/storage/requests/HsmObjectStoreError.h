#pragma once

#include "ObjectStoreError.h"

namespace hestia {
enum class HsmObjectStoreErrorCode {
    NO_ERROR,
    ERROR,
    CONFIG_ERROR,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    BASE_OBJECT_STORE_ERROR,
    MAX_ERROR
};

class HsmObjectStoreError : public RequestError<HsmObjectStoreErrorCode> {
  public:
    HsmObjectStoreError();
    HsmObjectStoreError(
        HsmObjectStoreErrorCode code, const std::string& message);
    HsmObjectStoreError(
        const ObjectStoreError& error, const std::string& message);

    std::string to_string() const override;

  private:
    std::string code_as_string() const override;
    static std::string code_to_string(HsmObjectStoreErrorCode code);
    ObjectStoreError m_base_object_store_error;
};
}  // namespace hestia