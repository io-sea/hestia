#pragma once

#include "CrudRequest.h"
#include "HestiaRequest.h"
#include "HestiaTypes.h"
#include "HsmAction.h"

#include "EnumUtils.h"

#include <filesystem>

namespace hestia {
class IConsoleInterface;

class HestiaClientCommand {
  public:
    STRINGABLE_ENUM(BodyFormat, NONE, ID, JSON, KEY_VALUE)

    CrudMethod get_crud_method() const;

    std::vector<std::string> get_system_subjects() const;

    bool is_crud_method() const;

    void parse_console_inputs(
        const IConsoleInterface& console, HestiaRequest& req) const;

    void parse_action(HestiaRequest& req);

    void set_crud_method(const std::string& method);

    void set_hsm_action(HsmAction::Action action);

    void set_subject(HestiaType subject);

    void set_hsm_subject(const std::string& subject);

    void set_system_subject(const std::string& subject);

    HsmAction m_action;
    CrudMethod m_crud_method{CrudMethod::READ};

    uint8_t m_source_tier{0};
    uint8_t m_target_tier{0};

    unsigned m_verbosity{2};
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

  private:
    void parse_create_update_inputs(
        const IConsoleInterface& console, HestiaRequest& req) const;

    void parse_read_inputs(
        const IConsoleInterface& console, HestiaRequest& req) const;

    void parse_remove_inputs(
        const IConsoleInterface& console, HestiaRequest& req) const;

    CrudIdentifier::InputFormat read_id_format(
        CrudIdentifier::InputFormat default_format) const;

    BodyFormat read_input_format(BodyFormat default_format) const;

    CrudQuery::BodyFormat read_output_format(BodyFormat default_format) const;

    void read_ids_from_console(
        const IConsoleInterface& console,
        HestiaRequest& req,
        CrudIdentifier::InputFormat default_format) const;
};

}  // namespace hestia