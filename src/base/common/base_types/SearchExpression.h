#pragma once

#include <string>

namespace hestia {
class SearchExpression {
  public:
    SearchExpression(const std::string& path) : m_path(path) {}

    std::string m_path;
};
}  // namespace hestia