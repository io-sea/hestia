#include "CrudResponse.h"

#include "JsonUtils.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace hestia {
CrudResponse::CrudResponse(
    const BaseRequest& request,
    const std::string& type,
    const CrudQuery::OutputFormat& output_format) :
    Response<CrudErrorCode>(request),
    m_output_format(output_format),
    m_type(type)
{
    CrudAttributes::FormatSpec attr_format;
    if (m_output_format.m_body_format == CrudQuery::BodyFormat::DICT) {
        attr_format.set_is_dict();
    }
    else if (m_output_format.m_body_format == CrudQuery::BodyFormat::JSON) {
        attr_format.set_is_json();
    }
    else if (
        m_output_format.m_body_format == CrudQuery::BodyFormat::NONE
        || m_output_format.m_body_format == CrudQuery::BodyFormat::ID) {
        attr_format.m_type = CrudAttributes::Format::NONE;
    }
    m_attributes.set_format(attr_format);
}

CrudResponse::Ptr CrudResponse::create(
    const BaseRequest& request,
    const std::string& type,
    const CrudQuery::OutputFormat& output_format)
{
    return std::make_unique<CrudResponse>(request, type, output_format);
}

CrudResponse::~CrudResponse() {}

void CrudResponse::add_attributes(
    const std::string& buffer, const CrudAttributes::FormatSpec& format)
{
    m_attributes.append(buffer, format);
    m_ids = CrudIdentifierCollection(m_attributes.get_ids());
}

void CrudResponse::add_attributes(
    std::unique_ptr<Dictionary> dict,
    const CrudIdentifier::FormatSpec& id_format)
{
    m_ids.load(*dict, id_format);
    m_attributes.set_body(std::move(dict));
}

void CrudResponse::add_ids(
    const std::string& json, const CrudIdentifier::FormatSpec& format)
{
    std::vector<std::string> primary_keys;
    JsonUtils::read_values(json, format.m_primary_key_name, primary_keys);
    m_ids.load_primary_keys(primary_keys);
}

const CrudAttributes& CrudResponse::get_attributes() const
{
    return m_attributes;
}

bool CrudResponse::expects_attributes() const
{
    return m_output_format.m_body_format == CrudQuery::BodyFormat::JSON
           || m_output_format.m_body_format == CrudQuery::BodyFormat::DICT;
}

bool CrudResponse::expects_items() const
{
    return m_output_format.m_body_format == CrudQuery::BodyFormat::ITEM;
}

bool CrudResponse::expects_ids() const
{
    return m_output_format.m_body_format == CrudQuery::BodyFormat::ID;
}

Model* CrudResponse::get_item() const
{
    if (m_items.empty()) {
        throw std::runtime_error(
            "Attempted to get item of type " + m_type
            + " in CrudResponse but none set. Use found() to check first");
    }
    return m_items[0].get();
}

const VecModelPtr& CrudResponse::items() const
{
    return m_items;
}

VecModelPtr& CrudResponse::items()
{
    return m_items;
}

bool CrudResponse::has_attributes() const
{
    return m_attributes.has_content();
}

bool CrudResponse::found() const
{
    return !m_items.empty() || !m_ids.empty() || has_attributes();
}

void CrudResponse::set_attributes(JsonDocument::Ptr json)
{
    m_attributes.set_body(std::move(json));
}

void CrudResponse::add_item(std::unique_ptr<Model> item)
{
    assert(item != nullptr);
    m_items.push_back(std::move(item));
}

void CrudResponse::set_locked(bool is_locked)
{
    m_locked = is_locked;
}

bool CrudResponse::locked() const
{
    return m_locked;
}

CrudIdentifierCollection& CrudResponse::ids()
{
    return m_ids;
}

const CrudIdentifierCollection& CrudResponse::get_ids() const
{
    return m_ids;
}

Dictionary& CrudResponse::modified_attrs()
{
    return m_modified_attrs;
}

const Dictionary& CrudResponse::modified_attrs() const
{
    return m_modified_attrs;
}

void CrudResponse::write(
    std::string& buffer, const CrudQuery::FormatSpec& format)
{
    if (m_output_format.m_body_format == CrudQuery::BodyFormat::ID) {
        m_ids.write(buffer, format.m_id_format);
    }
    else {
        m_attributes.write(buffer, format.m_attrs_format);
    }
}

}  // namespace hestia