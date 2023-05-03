#pragma once

#include <string>

namespace hestia {
struct HttpPreamble {
    std::string m_method;
    std::string m_path;
    std::string m_version;
};
}  // namespace hestia