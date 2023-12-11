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
        const CrudResponse& response,
        const std::string& source            = {},
        std::chrono::microseconds event_time = std::chrono::microseconds(0));

    CrudEvent(
        const std::string& subject_type,
        CrudMethod method,
        const std::vector<std::string>& ids,
        std::chrono::microseconds event_time = std::chrono::microseconds(0));

    static std::string get_type() { return s_type; }

    const std::string& get_subject_type() const;

    CrudMethod get_method() const;

    const CrudUserContext& get_user_context() const;

    const std::vector<std::string>& get_ids() const;

    const Dictionary& get_modified_attrs() const;

    const std::string& get_source() const;

    std::chrono::microseconds get_time() const;

  private:
    void init();

    static constexpr char s_type[]{"event"};
    StringSequenceField m_subject_ids{"subject_ids"};
    ScalarMapField m_updated_fields{"updated_fields"};
    StringField m_updated_attr_string{"updated_attr_string"};
    StringField m_subject_type{"subject_type"};
    EnumField<CrudMethod, CrudMethod_enum_string_converter> m_method{"method"};
    Dictionary m_modified_attrs;
    CrudUserContext m_user_context;
    std::string m_source;
    std::chrono::microseconds m_time{0};
    std::vector<std::string> m_ids;
};
}  // namespace hestia
