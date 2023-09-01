#include "HttpCrudPath.h"

namespace hestia {
void HttpCrudPath::from_identifier(const CrudIdentifier& id, std::string& path)
{
    if (id.has_primary_key()) {
        path += id.get_primary_key();
    }
    else if (id.has_name()) {
        path += "?name=" + id.get_name();
        if (id.has_parent_name()) {
            path += ",parent_name=" + id.get_parent_name();
        }
        else if (id.has_parent_primary_key()) {
            path += ",parent_id=" + id.get_parent_primary_key();
        }
    }
    else if (id.has_parent_primary_key()) {
        path += "?parent_id=" + id.get_parent_primary_key();
    }
}

void HttpCrudPath::from_filter(const Map& map, std::string& path)
{
    if (map.empty()) {
        return;
    }

    path += "?";
    std::size_t count{0};
    for (const auto& [key, value] : map.data()) {
        path += key + "=" + value;
        if (count < map.data().size() - 1) {
            path += ",";
        }
        count++;
    }
}

void HttpCrudPath::from_query(const CrudQuery& query, std::string& path)
{
    if (query.is_filter()) {
        from_filter(query.get_filter(), path);
    }
    else if (query.is_id()) {
        if (query.has_single_id()) {
            from_identifier(query.get_id(), path);
        }
    }
}
}  // namespace hestia