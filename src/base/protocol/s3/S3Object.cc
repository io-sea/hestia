#include "S3Object.h"

#include "XmlDocument.h"
#include "XmlElement.h"

#include <ctime>

namespace hestia {

S3Object::RestoreStatus::RestoreStatus(const XmlElement& element)
{
    for (const auto& child : element.get_children()) {
        if (child->get_tag_name() == "IsRestoreInProgress") {
            m_restore_in_progress = child->get_text() == "TRUE";
        }
        else if (child->get_tag_name() == "RestoreExpiryDate") {
            m_restore_expiry_date = S3Timestamp(child->get_text());
        }
    }
}

XmlElementPtr S3Object::RestoreStatus::to_xml() const
{
    auto root = XmlElement::create("RestoreStatus");

    if (m_activated) {
        auto e = XmlElement::create("IsRestoreInProgress");
        e->set_text(m_restore_in_progress ? "TRUE" : "FALSE");
        root->add_child(std::move(e));
    }
    if (!m_restore_expiry_date.m_value.empty()) {
        auto e = XmlElement::create("RestoreExpiryDate");
        e->set_text(m_restore_expiry_date.m_value);
        root->add_child(std::move(e));
    }
    return root;
}

S3Object::S3Object(const std::string& name) : m_key(name) {}

S3Object::S3Object(const XmlElement& element)
{
    for (const auto& child : element.get_children()) {
        if (child->get_tag_name() == "ChecksumAlgorithm") {
            m_checksum_algorithm = child->get_text();
        }
        else if (child->get_tag_name() == "ETag") {
            m_etag = child->get_text();
        }
        else if (child->get_tag_name() == "Key") {
            m_key = child->get_text();
        }
        else if (child->get_tag_name() == "LastModified") {
            m_last_modified = S3Timestamp(child->get_text());
        }
        else if (child->get_tag_name() == "Owner") {
            m_owner = S3Owner(*child);
        }
        else if (child->get_tag_name() == "RestoreStatus") {
            m_restore_status = RestoreStatus(*child);
        }
        else if (child->get_tag_name() == "Size") {
            m_size = std::stoull(child->get_text());
        }
        else if (child->get_tag_name() == "StorageClass") {
            m_storage_class = child->get_text();
        }
    }
}

XmlElementPtr S3Object::to_xml() const
{
    auto root = XmlElement::create("Object");
    to_xml(*root);
    return root;
}

void S3Object::to_xml(XmlElement& element) const
{
    if (!m_checksum_algorithm.empty()) {
        auto e = XmlElement::create("ChecksumAlgorithm");
        e->set_text(m_checksum_algorithm);
        element.add_child(std::move(e));
    }
    if (!m_etag.empty()) {
        auto e = XmlElement::create("ETag");
        e->set_text(m_etag);
        element.add_child(std::move(e));
    }
    if (!m_key.empty()) {
        auto e = XmlElement::create("Key");
        e->set_text(m_key);
        element.add_child(std::move(e));
    }
    if (!m_last_modified.m_value.empty()) {
        auto e = XmlElement::create("LastModified");
        e->set_text(m_last_modified.m_value);
        element.add_child(std::move(e));
    }
    if (m_owner.populated()) {
        element.add_child(m_owner.to_xml());
    }
    if (m_restore_status.populated()) {
        element.add_child(m_restore_status.to_xml());
    }
    if (m_size > 0) {
        auto e = XmlElement::create("Size");
        e->set_text(std::to_string(m_size));
        element.add_child(std::move(e));
    }
    if (!m_storage_class.empty()) {
        auto e = XmlElement::create("StorageClass");
        e->set_text(m_storage_class);
        element.add_child(std::move(e));
    }
}

std::string S3Object::to_string() const
{
    XmlDocument xml_doc;
    xml_doc.set_root(to_xml());
    return xml_doc.to_string();
}

bool S3Object::is_equal(const S3Object& other) const
{
    return m_key == other.m_key;
}
}  // namespace hestia