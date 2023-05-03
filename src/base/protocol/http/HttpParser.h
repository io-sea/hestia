#pragma once

#include "HttpPreamble.h"

namespace hestia {
class HttpParser {
  public:
    static bool parse_preamble(const std::string& line, HttpPreamble& preamble);
};
}  // namespace hestia