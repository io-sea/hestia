#include "CrudSerializer.h"

#include <iostream>

namespace hestia {
CrudSerializer::CrudSerializer(ModelSerializer::Ptr model_serializer) :
    m_model_serializer(std::move(model_serializer))
{
}

CrudSerializer::Ptr CrudSerializer::create(
    ModelSerializer::Ptr model_serializer)
{
    return std::make_unique<CrudSerializer>(std::move(model_serializer));
}

std::string CrudSerializer::get_type() const
{
    return m_model_serializer->get_type();
}

void CrudSerializer::load_template()
{
    m_model_serializer->load_template();
}

void CrudSerializer::append_to_dict(const Model& item, Dictionary& dict) const
{
    auto item_dict = std::make_unique<Dictionary>();
    m_model_serializer->to_dict(item, *item_dict);
    dict.add_sequence_item(std::move(item_dict));
}

ModelSerializer* CrudSerializer::get_model_serializer() const
{
    return m_model_serializer.get();
}

Model::Ptr CrudSerializer::create_template(
    const CrudRequest& req, std::size_t index) const
{
    auto item = m_model_serializer->create_template();
    if (req.has_attributes()) {
        item->deserialize(*req.get_attribute_as_dict(index));
    }
    return item;
}

bool CrudSerializer::type_has_owner() const
{
    return m_model_serializer->type_has_owner();
}

std::string CrudSerializer::get_runtime_type() const
{
    return m_model_serializer->get_runtime_type();
}

void CrudSerializer::to_json(
    const CrudRequest& req, std::string& buffer, const Index& index) const
{
    Dictionary dict;
    if (req.has_items()) {
        m_model_serializer->to_dict(req.items(), dict, {}, index);
    }
    else if (req.has_attributes()) {
        auto input_format = m_format.m_attr_format;
        input_format.set_is_json();
        req.write_body(input_format, buffer, index);
        return;
    }
    else {
        req.get_ids().write(dict, m_format.m_id_format.m_id_spec);
    }
    JsonDocument(dict).write(buffer, {}, m_format.m_attr_format.m_json_format);
}

void CrudSerializer::to_json(const Model& model, std::string& buffer) const
{
    Dictionary dict;
    m_model_serializer->to_dict(model, dict);
    JsonDocument(dict).write(buffer);
}

void CrudSerializer::append_json(
    const std::string& json_str, CrudResponse& response) const
{
    if (response.expects_items()) {
        Dictionary dict;
        JsonDocument(json_str).write(dict);
        m_model_serializer->from_dict(dict, response.items());
    }
    else if (response.expects_attributes() || response.expects_ids()) {
        auto input_format = m_format.m_attr_format;
        input_format.set_is_json();
        response.add_attributes(json_str, input_format);
    }
}

Model::Ptr CrudSerializer::create_template() const
{
    return m_model_serializer->create_template();
}

Model* CrudSerializer::get_template() const
{
    return m_model_serializer->get_template();
}

void CrudSerializer::append_dict(
    Dictionary::Ptr dict, CrudResponse& response, bool record_modified) const
{
    if (response.expects_items()) {
        VecModelPtr items;
        m_model_serializer->from_dict(*dict, items);
        for (auto& item : items) {
            response.add_item(std::move(item));
        }
    }
    else {
        if (record_modified) {
            if (dict->is_sequence()) {
                for (const auto& item : dict->get_sequence()) {
                    Map flat_attrs;
                    item->flatten(flat_attrs);
                    response.modified_attrs().push_back(flat_attrs);
                }
            }
            else {
                Map flat_attrs;
                dict->flatten(flat_attrs);
                response.modified_attrs().push_back(flat_attrs);
            }
        }

        CrudIdentifier::FormatSpec id_format;
        id_format.m_primary_key_name =
            m_model_serializer->get_template()->get_primary_key_name();
        response.add_attributes(std::move(dict), id_format);
    }
}

bool CrudSerializer::matches_query(const Model& item, const Map& query) const
{
    return m_model_serializer->matches_query(item, query);
}
}  // namespace hestia