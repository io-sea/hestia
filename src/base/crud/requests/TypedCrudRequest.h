#pragma once

#include "CrudRequest.h"

namespace hestia {

template<typename ItemT>
class TypedCrudRequest : public CrudRequest {
  public:
    TypedCrudRequest(
        CrudMethod method,
        const ItemT& item,
        const CrudQuery& query,
        const CrudUserContext& user_context) :
        CrudRequest(method, query, user_context)
    {
        m_items.emplace_back(std::make_unique<ItemT>(item));
    };

    TypedCrudRequest(
        CrudMethod method,
        const ItemT& item,
        const CrudUserContext& user_context) :
        CrudRequest(method, user_context)
    {
        if (const auto primary_key = item.get_primary_key();
            !primary_key.empty()) {
            m_query.set_ids(CrudIdentifierCollection({primary_key}));
        }
        m_items.emplace_back(std::make_unique<ItemT>(item));
    };

    virtual ~TypedCrudRequest() = default;
};
}  // namespace hestia