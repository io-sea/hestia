#pragma once

#include "CrudQuery.h"
#include "EnumUtils.h"
#include "Request.h"

namespace hestia {

STRINGABLE_ENUM(
    CrudMethod, CREATE, READ, UPDATE, REMOVE, IDENTIFY, LOCK, UNLOCK, LOCKED)

enum class CrudLockType { READ, WRITE };

class BaseCrudRequest : public BaseRequest {
  public:
    BaseCrudRequest(
        const std::string& user_id       = {},
        const VecCrudIdentifier& ids     = {},
        const CrudAttributes& attributes = {},
        CrudQuery::OutputFormat output_format =
            CrudQuery::OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON);

    BaseCrudRequest(const std::string& user_id, const CrudQuery& query);

    BaseCrudRequest(const std::string& user_id, CrudLockType lock_type);

    virtual ~BaseCrudRequest() = default;

    const CrudAttributes& get_attributes() const;

    const VecCrudIdentifier& get_ids() const;

    const std::string& get_user_id() const;

    const CrudQuery& get_query() const;

    CrudLockType lock_type() const;

    static const std::vector<CrudMethod> s_all_methods;

  protected:
    const std::string& m_user_id;
    CrudQuery m_query;
    VecCrudIdentifier m_ids;
    CrudLockType m_lock_type{CrudLockType::READ};
};
}  // namespace hestia