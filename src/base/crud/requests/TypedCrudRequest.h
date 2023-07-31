#pragma once

#include "CrudRequest.h"

namespace hestia {

template<typename ItemT>
class TypedCrudRequest : public CrudRequest {
  public:
    TypedCrudRequest(
        CrudMethod method,
        const ItemT& item,
        const std::string& user_id,
        CrudQuery::OutputFormat output_format =
            CrudQuery::OutputFormat::ATTRIBUTES,
        CrudAttributes::Format attributes_format =
            CrudAttributes::Format::JSON) :
        CrudRequest(method, user_id, {}, {}, output_format, attributes_format)
    {
        m_items.emplace_back(std::make_unique<ItemT>(item));
    };

    virtual ~TypedCrudRequest() = default;
};
}  // namespace hestia