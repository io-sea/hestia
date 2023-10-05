#pragma once

#include "XmlDocument.h"

#include <string>

namespace hestia {
class XmlParser {
  public:
    XmlDocument::Ptr run(const std::string& content) const;
};
}  // namespace hestia