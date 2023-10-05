#pragma once

#include "S3Error.h"
#include "S3Owner.h"

#include <memory>
#include <string>

namespace hestia {

class XmlElement;
using XmlElementPtr = std::unique_ptr<XmlElement>;

class S3Bucket {
  public:
    S3Bucket(const std::string& name);

    S3Bucket(const XmlElement& element);
    using Ptr = std::unique_ptr<S3Bucket>;

    bool operator==(const S3Bucket& other) const
    {
        return m_name == other.m_name;
    }

    const std::string& name() const { return m_name; }

    S3Error validate_name(const std::string& tracking_id) const;

    const std::string& get_location_constraint() const;

    XmlElementPtr get_location_constraint_xml() const;

  private:
    std::string m_location_constraint;
    std::string m_name;
    std::string m_creation_date;
};
}  // namespace hestia