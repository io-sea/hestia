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

    virtual ~TypedCrudRequest() = default;
};
}  // namespace hestia