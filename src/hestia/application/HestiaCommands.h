#pragma once

#include "CrudRequest.h"
#include "HestiaTypes.h"
#include "HsmAction.h"

#include "EnumUtils.h"

#include <filesystem>

namespace hestia {
class IConsoleInterface;

class HestiaClientCommand {

  public:
    STRINGABLE_ENUM(OutputFormat, NONE, ID, JSON, KEY_VALUE, ID_KEY_VALUE)

    std::vector<std::string> get_crud_methods() const;

    std::vector<std::string> get_action_subjects() const;

    std::vector<HsmAction::Action> get_subject_actions(
        const std::string& subject) const;

    std::vector<std::string> get_hsm_subjects() const;

    std::vector<std::string> get_system_subjects() const;

    bool is_crud_method() const;

    bool is_create_method() const;

    bool is_read_method() const;

    bool is_update_method() const;

    bool is_remove_method() const;

    bool is_identify_method() const;

    bool is_data_management_action() const;

    bool is_data_io_action() const;

    bool is_data_put_action() const;

    static bool expects_id(OutputFormat format);

    static bool expects_attributes(OutputFormat format);

    static OutputFormat output_format_from_string(const std::string& format);

    std::pair<OutputFormat, CrudAttributes::Format> parse_create_update_inputs(
        VecCrudIdentifier& ids,
        CrudAttributes& attributes,
        IConsoleInterface* console) const;

    std::pair<OutputFormat, CrudAttributes::Format> parse_read_inputs(
        CrudQuery& query, IConsoleInterface* console) const;

    void parse_remove_inputs(
        VecCrudIdentifier& ids, IConsoleInterface* console) const;

    void set_crud_method(const std::string& method);

    void set_hsm_action(HsmAction::Action action);

    void set_subject(HestiaType subject) { m_subject = subject; }

    void set_hsm_subject(const std::string& subject);

    void set_system_subject(const std::string& subject);

    HsmAction m_action;
    CrudMethod m_crud_method{CrudMethod::READ};

    uint8_t m_source_tier{0};
    uint8_t m_target_tier{0};

    bool m_is_verbose{false};
    bool m_is_version{false};

    HestiaType m_subject;

    std::vector<std::string> m_id;
    std::vector<std::string> m_body;
    std::string m_id_format;
    std::string m_input_format;
    std::string m_output_format;

    std::string m_offset;
    std::string m_count;

    std::filesystem::path m_path;
};

}  // namespace hestia