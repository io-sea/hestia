#pragma once

#include "HsmObject.h"

#include <string>
#include <vector>

namespace hestia {
class Dataset {
  public:
    Dataset() = default;

    Dataset(const std::string& id) : m_identifier(id) {}

    const std::string& id() const { return m_identifier; }

    std::string m_identifier;
    std::vector<HsmObject> m_objects;
};
}  // namespace hestia