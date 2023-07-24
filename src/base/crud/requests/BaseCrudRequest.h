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
        const VecCrudIdentifier& ids     = {},
        const CrudAttributes& attributes = {},
        CrudQuery::OutputFormat output_format =
            CrudQuery::OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON);

    BaseCrudRequest(const CrudQuery& query);

    BaseCrudRequest(CrudLockType lock_type);

    virtual ~BaseCrudRequest() = default;

    const CrudAttributes& get_attributes() const;

    const VecCrudIdentifier& get_ids() const;

    const CrudQuery& get_query() const;

    CrudLockType lock_type() const;

    static const std::vector<CrudMethod> s_all_methods;

  protected:
    CrudQuery m_query;
    VecCrudIdentifier m_ids;
    CrudLockType m_lock_type{CrudLockType::READ};
};
}  // namespace hestia