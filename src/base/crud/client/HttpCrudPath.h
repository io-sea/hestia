#pragma once

#include "CrudQuery.h"

namespace hestia {
class HttpCrudPath {
  public:
    static void from_identifier(
        const CrudIdentifier& id, std::string& path, Map& queries);

    static void from_filter(const Map& map, Map& queries);

    static void from_query(
        const CrudQuery& query, std::string& path, Map& queries);
};
}  // namespace hestia