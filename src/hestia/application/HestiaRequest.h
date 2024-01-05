#pragma once

#include "CrudAttributes.h"
#include "CrudRequest.h"
#include "HestiaTypes.h"
#include "HsmAction.h"
#include "Request.h"

namespace hestia {

class HestiaRequest : public BaseRequest {
  public:
    enum class Type { CRUD, HSM_ACTION };

    HestiaRequest(HestiaType subject, CrudMethod method);

    HestiaRequest(HestiaType subject, const HsmAction& action);

    HsmAction& action();

    void append(const std::string& body, CrudAttributes::FormatSpec format);

    const HsmAction& get_action() const;

    const CrudRequest& get_crud_request() const;

    const CrudQuery::OutputFormat& get_output_format() const;

    std::string get_hsm_type_as_string() const;

    bool is_crud_request() const;

    bool is_hsm_action() const;

    bool is_create_method() const;

    bool is_read_method() const;

    bool is_identify_method() const;

    bool is_update_method() const;

    bool is_remove_method() const;

    bool is_create_or_update_method() const;

    bool is_system_type() const;

    bool is_hsm_type() const;

    bool is_user_type() const;

    void set_ids(const CrudIdentifierCollection& ids);

    void set_offset_and_count(std::size_t offset, std::size_t count);

    void set_query_filter(const Map& filter);

    void set_output_format(const CrudQuery::OutputFormat& format);

    bool supports_stream_source() const;

    bool supports_stream_sink() const;

  private:
    HestiaType m_subject;
    Type m_type{Type::CRUD};
    CrudRequest m_crud_request;
    HsmAction m_action;
};
}  // namespace hestia