#pragma once

#include <string>

#include "HttpError.h"

namespace hestia {
class S3Error : public HttpError {
  public:
    enum class Code {
        _400_INCOMPLETE_BODY,
        _400_INVALID_ARGUMENT,
        _400_INVALID_BUCKET_NAME,
        _400_INVALID_SIGNATURE_TYPE,
        _400_AUTHORIZATION_HEADER_MALFORMED,
        _403_INVALID_KEY_ID,
        _403_ACCESS_DENIED,
        _403_SIGNATURE_DOES_NOT_MATCH,
        _404_NO_SUCH_BUCKET,
        _404_NO_SUCH_KEY,
        _404_NO_SUCH_VERSION,
        _404_NOT_IMPLEMENTED,
        _409_BUCKET_EXISTS,
        _409_BUCKET_NOT_EMPTY,
        _411_MISSING_CONTENT_LENGTH,
        _500_INTERNAL_SERVER_ERROR,
        CUSTOM
    };

    S3Error(Code code, const std::string& request);

    std::string to_string() const override;

  private:
    Code m_s3_code;
    std::string m_identifier;
    std::string m_request;
};
}  // namespace hestia
