#pragma once

#include "HttpPreamble.h"
#include "Map.h"

namespace hestia {
class HttpParser {
  public:
    static bool parse_preamble(const std::string& line, HttpPreamble& preamble);

    static bool parse_form_data(const std::string& input, Map& parsed);
};
}  // namespace hestia