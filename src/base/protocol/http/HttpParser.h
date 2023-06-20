#pragma once

#include "HttpPreamble.h"
#include "Metadata.h"

namespace hestia {
class HttpParser {
  public:
    static bool parse_preamble(const std::string& line, HttpPreamble& preamble);

    static bool parse_form_data(const std::string& input, Metadata& parsed);
};
}  // namespace hestia