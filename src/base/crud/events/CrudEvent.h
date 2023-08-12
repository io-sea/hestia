#pragma once

#include "CrudResponse.h"

namespace hestia {
class CrudEvent : public Model {
  public:
    CrudEvent();

    CrudEvent(
        CrudMethod method, const CrudRequest& request, CrudResponse& response);

    static std::string get_type() { return s_type; }

  private:
    void init();

    static constexpr char s_type[]{"event"};
    ScalarSequenceField<std::vector<std::string>> m_subject_ids{"subject_ids"};
    ScalarSequenceField<std::vector<std::string>> m_parent_ids{"parent_ids"};
    ScalarMapField m_updated_fields{"updated_fields"};
    StringField m_updated_attr_string{"updated_attr_string"};
    EnumField<CrudMethod, CrudMethod_enum_string_converter> m_method{"method"};
};
}  // namespace hestia
