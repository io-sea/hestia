#pragma once

#include <string>
#include <vector>

#include "HttpResponse.h"
#include "HttpStatus.h"
#include "S3Request.h"
#include "S3StatusCode.h"

namespace hestia {

class XmlElement;
using XmlElementPtr = std::unique_ptr<XmlElement>;

class S3Status : public HttpStatus {
  public:
    S3Status() = default;

    S3Status(
        S3StatusCode code,
        const S3Request& request,
        const std::string& message_details = {});

    S3Status(const HttpResponse& http_response);

    S3StatusCode get_s3_code() const;

    void add_field(const std::string& key, const std::string& value);

    void add_message_details(const std::string& details);

    std::string to_string() const override;

    bool is_ok() const;

  private:
    void from_xml(const XmlElement& element);

    XmlElementPtr to_xml() const;

    S3StatusCode m_s3_code;
    std::string m_code_str;
    std::string m_request_id;
    S3Path m_path;
    std::string m_message_details;
    std::vector<std::pair<std::string, std::string>> m_fields;
};
}  // namespace hestia
