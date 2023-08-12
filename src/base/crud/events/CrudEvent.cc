#include "CrudAttributes.h"
#include "CrudEvent.h"
#include "Dictionary.h"
#include <memory>

namespace hestia {
CrudEvent::CrudEvent() : Model(s_type)
{
    init();
}

void CrudEvent::init()
{
    m_name.set_index_on(true);

    register_scalar_field(&m_method);

    register_sequence_field(&m_subject_ids);
    register_sequence_field(&m_parent_ids);

    register_scalar_field(&m_updated_attr_string);
    register_map_field(&m_updated_fields);
}

CrudEvent::CrudEvent(
    CrudMethod method, const CrudRequest& request, CrudResponse& response) :
    Model(s_type)
{
    init();
    m_method.update_value(method);

    for (const auto& item : response.items()) {
        m_subject_ids.get_container_as_writeable().push_back(
            item->get_primary_key());

        if (item->has_owner() && !m_foreign_key_fields.empty()) {
            auto parent_type = item->get_parent_type();
            if (m_foreign_key_fields.size() == 1) {
                m_parent_ids.get_container_as_writeable().push_back(
                    m_foreign_key_fields.begin()->second->get_id());
            }
            else {
                for (const auto& [key, field] : m_foreign_key_fields) {
                    if (field->is_parent()) {
                        m_parent_ids.get_container_as_writeable().push_back(
                            field->get_id());
                        break;
                    }
                }
            }
        }
        else {
            m_parent_ids.get_container_as_writeable().push_back("");
        }
    }

    // Determine updated fields from the request and response
    Dictionary dict;
    switch (request.get_query().get_output_format()) {
        case CrudQuery::OutputFormat::ATTRIBUTES:
            m_updated_attr_string.update_value(
                response.attributes().get_buffer());
            break;
        case CrudQuery::OutputFormat::DICT:
            m_updated_fields.deserialize(*response.dict(), Format::MODIFIED);
            break;
        case CrudQuery::OutputFormat::ITEM:
            response.get_item()->serialize(dict, Format::MODIFIED);
            m_updated_fields.deserialize(dict);
            break;
        case CrudQuery::OutputFormat::ID:
            break;
    }
}

}  // namespace hestia