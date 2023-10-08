#pragma once

#include "Map.h"
#include "S3Owner.h"
#include "S3Types.h"
#include <string>

namespace hestia {

class XmlElement;
using XmlElementPtr = std::unique_ptr<XmlElement>;

class S3Object {
  public:
    struct RestoreStatus {
        RestoreStatus() = default;
        RestoreStatus(const XmlElement& element);

        XmlElementPtr to_xml() const;

        bool populated() const { return m_activated; }

        bool m_activated{false};
        bool m_restore_in_progress{false};
        S3Timestamp m_restore_expiry_date;
    };

    S3Object(const std::string& name = {});

    S3Object(const XmlElement& element);
    using Ptr = std::unique_ptr<S3Object>;

    bool is_equal(const S3Object& other) const;

    std::string to_string() const;

    XmlElementPtr to_xml() const;

    void to_xml(XmlElement& element) const;

    bool operator==(const S3Object& other) const { return is_equal(other); }

    friend std::ostream& operator<<(std::ostream& os, S3Object const& obj)
    {
        os << obj.to_string();
        return os;
    }

    Map metadata;
    std::string m_bucket;
    S3Owner m_owner;
    std::string m_checksum_algorithm;
    std::string m_etag;
    std::string m_key;
    S3Timestamp m_last_modified;
    RestoreStatus m_restore_status;
    std::size_t m_size{0};
    std::string m_storage_class;
    std::string m_content_mimetype{"binary/octet-stream"};
};
}  // namespace hestia