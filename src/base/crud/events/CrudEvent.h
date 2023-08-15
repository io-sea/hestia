#pragma once

#include "CrudResponse.h"
#include "Map.h"

namespace hestia {
class CrudEvent : public Model {
  public:
    CrudEvent();

    CrudEvent(
        const std::string& subject_type,
        CrudMethod method,
        const CrudRequest& request,
        CrudResponse& response);

    static std::string get_type() { return s_type; }

    const std::string& get_subject_type() const;

    CrudMethod get_method() const;

    const std::vector<std::string>& get_ids() const;

    const std::vector<Map>& get_modified_attrs() const;

  private:
    void init();

    static constexpr char s_type[]{"event"};
    ScalarSequenceField<std::vector<std::string>> m_subject_ids{"subject_ids"};
    ScalarSequenceField<std::vector<std::string>> m_parent_ids{"parent_ids"};
    ScalarMapField m_updated_fields{"updated_fields"};
    StringField m_updated_attr_string{"updated_attr_string"};
    StringField m_subject_type{"subject_type"};
    EnumField<CrudMethod, CrudMethod_enum_string_converter> m_method{"method"};
    std::vector<Map> m_modified_attrs;
};
}  // namespace hestia
