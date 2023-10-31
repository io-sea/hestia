#pragma once

#include "CrudIdentifier.h"
#include "CrudResponse.h"
#include "ErrorUtils.h"
#include "HsmActionResponse.h"

namespace hestia {
class HestiaResponse {
  public:
    HestiaResponse(CrudResponse::Ptr crud_response);

    HestiaResponse(HsmActionResponse::Ptr action_response);

    HestiaResponse(OpStatus status);

    using Ptr = std::unique_ptr<HestiaResponse>;

    static Ptr create(CrudResponse::Ptr crud_response);

    static Ptr create(HsmActionResponse::Ptr action_response);

    static Ptr create(OpStatus status);

    std::string get_action_id() const;

    std::string get_attributes_as_string() const;

    const CrudIdentifierCollection& get_ids() const;

    const OpStatus& get_status() const;

    bool has_ids() const;

    bool has_attributes() const;

    bool has_content() const;

    bool is_crud_response() const;

    bool ok() const;

    std::string write(const CrudQuery::FormatSpec& format = {}) const;

  private:
    OpStatus m_status;
    CrudResponse::Ptr m_crud_response;
    HsmActionResponse::Ptr m_action_response;
};
}  // namespace hestia