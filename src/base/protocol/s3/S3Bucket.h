#pragma once

#include "S3Owner.h"
#include "S3Status.h"
#include "S3Types.h"

#include <memory>
#include <string>

namespace hestia {

class XmlElement;
using XmlElementPtr = std::unique_ptr<XmlElement>;

class S3Bucket {
  public:
    S3Bucket() = default;

    S3Bucket(const std::string& name, const S3Timestamp& creation_date = {});

    S3Bucket(const XmlElement& element);
    using Ptr = std::unique_ptr<S3Bucket>;

    const std::string& name() const { return m_name; }

    S3Status validate_name(
        const std::string& tracking_id,
        const S3UserContext& user_context) const;

    XmlElementPtr to_xml() const;

    const std::string& get_location_constraint() const;

    void add_location_constraint(XmlElement& element) const;

    bool operator==(const S3Bucket& other) const
    {
        return m_name == other.m_name;
    }

  private:
    std::string m_location_constraint;
    std::string m_name;
    S3Timestamp m_creation_date;
};
}  // namespace hestia