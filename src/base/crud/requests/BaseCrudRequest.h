#pragma once

#include "AuthorizationContext.h"
#include "CrudQuery.h"
#include "EnumUtils.h"
#include "Request.h"

#include <array>

namespace hestia {

STRINGABLE_ENUM(
    CrudMethod, CREATE, READ, UPDATE, REMOVE, IDENTIFY, LOCK, UNLOCK, LOCKED)

enum class CrudLockType { READ, WRITE };

struct CrudUserContext {
    CrudUserContext(const AuthorizationContext& auth) :
        m_id(auth.m_user_id), m_token(auth.m_user_token)
    {
    }

    CrudUserContext(const std::string& id = {}, const std::string& token = {}) :
        m_id(id), m_token(token)
    {
    }

    std::string m_id;
    std::string m_token;
};

class BaseCrudRequest : public BaseRequest {
  public:
    BaseCrudRequest() = default;

    BaseCrudRequest(
        const CrudUserContext& user_context, const CrudQuery& query);

    BaseCrudRequest(
        const CrudUserContext& user_context, CrudLockType lock_type);

    virtual ~BaseCrudRequest() = default;

    void append(
        const std::string& body, const CrudAttributes::FormatSpec& format);

    const CrudAttributes& get_attributes() const;

    static std::vector<std::string> get_crud_methods();

    const CrudIdentifierCollection& get_ids() const;

    const CrudUserContext& get_user_context() const;

    const CrudQuery& get_query() const;

    CrudQuery::BodyFormat get_output_format() const;

    CrudLockType lock_type() const;

    void set_ids(const CrudIdentifierCollection& ids);

    void set_output_format(CrudQuery::BodyFormat format);

    void set_query_filter(const Map& filter);

    void set_ids(
        const std::string& buffer,
        const CrudIdentifierCollection::FormatSpec& format);

    void set_offset_and_count(std::size_t offset, std::size_t count);

    static constexpr std::array<CrudMethod, 8> s_all_methods{
        CrudMethod::CREATE, CrudMethod::READ,     CrudMethod::UPDATE,
        CrudMethod::REMOVE, CrudMethod::IDENTIFY, CrudMethod::LOCK,
        CrudMethod::UNLOCK, CrudMethod::LOCKED};

  protected:
    CrudUserContext m_user_context;
    CrudQuery m_query;
    CrudLockType m_lock_type{CrudLockType::READ};
};
}  // namespace hestia