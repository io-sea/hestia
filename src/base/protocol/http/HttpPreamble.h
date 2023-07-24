#pragma once

#include "Map.h"

#include <string>

namespace hestia {
struct HttpPreamble {
    std::string m_method;
    std::string m_path;
    std::string m_version;
    Map m_queries;
};
}  // namespace hestia