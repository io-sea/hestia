#include "HttpCrudPath.h"

namespace hestia {
void HttpCrudPath::from_identifier(
    const CrudIdentifier& id, std::string& path, Map& queries)
{
    if (id.has_primary_key()) {
        path += "/" + id.get_primary_key();
    }
    else if (id.has_name()) {
        queries.set_item("name", id.get_name());
        if (id.has_parent_name()) {
            queries.set_item("parent_name", id.get_parent_name());
        }
        else if (id.has_parent_primary_key()) {
            queries.set_item("parent_id", id.get_parent_primary_key());
        }
    }
    else if (id.has_parent_primary_key()) {
        queries.set_item("parent_id", id.get_parent_primary_key());
    }
}

void HttpCrudPath::from_filter(const Map& map, Map& queries)
{
    for (const auto& [key, value] : map.data()) {
        queries.set_item(key, value);
    }
}

void HttpCrudPath::from_query(
    const CrudQuery& query, std::string& path, Map& queries)
{
    if (query.is_filter()) {
        from_filter(query.get_filter(), queries);
    }
    else if (query.is_id()) {
        if (query.get_ids().size() == 1) {
            from_identifier(query.get_ids().first(), path, queries);
        }
    }
}
}  // namespace hestia