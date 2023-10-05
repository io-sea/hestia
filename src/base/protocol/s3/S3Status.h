#pragma once

#include <string>
#include <vector>

#include "HttpResponse.h"
#include "HttpStatus.h"
#include "S3Request.h"

namespace hestia {

class XmlElement;
using XmlElementPtr = std::unique_ptr<XmlElement>;

class S3Status : public HttpStatus {
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

    S3Status() = default;

    S3Status(
        Code code,
        const S3Request& request,
        const std::string& message_details = {});

    S3Status(const HttpResponse& http_response);

    Code get_s3_code() const;

    void add_field(const std::string& key, const std::string& value);

    void add_message_details(const std::string& details);

    std::string to_string() const override;

    bool is_ok() const;

  private:
    void from_xml(const XmlElement& element);

    XmlElementPtr to_xml() const;

    Code m_s3_code;
    std::string m_code_str;
    std::string m_request_id;
    S3Path m_path;
    std::string m_message_details;
    std::vector<std::pair<std::string, std::string>> m_fields;
};
}  // namespace hestia
