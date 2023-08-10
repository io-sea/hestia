#pragma once

#include "CrudRequest.h"

namespace hestia {

template<typename ItemT>
class TypedCrudRequest : public CrudRequest {
  public:
    TypedCrudRequest(
        CrudMethod method,
        const ItemT& item,
        const CrudUserContext& user_context,
        CrudQuery::OutputFormat output_format =
            CrudQuery::OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON) :
        CrudRequest(
            method, user_context, {}, {}, output_format, attributes_format)
    {
        m_items.emplace_back(std::make_unique<ItemT>(item));
    };

    virtual ~TypedCrudRequest() = default;
};
}  // namespace hestia