#pragma once

#include "CrudQuery.h"

namespace hestia {
class HttpCrudPath {
  public:
    static void from_identifier(const CrudIdentifier& id, std::string& path);

    static void from_filter(const Map& map, std::string& path);

    static void from_query(const CrudQuery& query, std::string& path);
};
}  // namespace hestia