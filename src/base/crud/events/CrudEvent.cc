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
    register_scalar_field(&m_method);

    register_sequence_field(&m_subject_ids);
    register_sequence_field(&m_parent_ids);

    register_scalar_field(&m_updated_attr_string);
    register_map_field(&m_updated_fields);
}

CrudEvent::CrudEvent(
    const std::string& subject_type,
    CrudMethod method,
    const CrudRequest&,
    CrudResponse& response) :
    Model(s_type)
{
    init();

    m_subject_type.init_value(subject_type);
    m_method.init_value(method);

    m_subject_ids.get_container_as_writeable() = response.ids();
    m_parent_ids.get_container_as_writeable()  = response.parent_ids();
    m_modified_attrs                           = response.modified_attrs();
}

const std::vector<std::string>& CrudEvent::get_ids() const
{
    return m_subject_ids.container();
}

const std::vector<Map>& CrudEvent::get_modified_attrs() const
{
    return m_modified_attrs;
}

const std::string& CrudEvent::get_subject_type() const
{
    return m_subject_type.get_value();
}

CrudMethod CrudEvent::get_method() const
{
    return m_method.get_value();
}

}  // namespace hestia