#include "HttpParser.h"

#include "HttpParser.h"

namespace hestia {
bool HttpParser::parse_preamble(const std::string& line, HttpPreamble& preamble)
{
    bool in_path{false};
    bool in_method{true};
    bool in_protocol{false};

    for (const auto c : line) {
        if (in_path) {
            if (std::isspace(c) != 0) {
                in_path   = false;
                in_method = true;
            }
            else {
                preamble.m_path.push_back(c);
            }
        }
        else if (in_method) {
            if (std::isspace(c) != 0) {
                in_method   = false;
                in_protocol = true;
            }
            else {
                preamble.m_method.push_back(c);
            }
        }
        else if (in_protocol) {
            preamble.m_version.push_back(c);
        }
    }
    if (preamble.m_path.empty()) {
        preamble.m_path = "/";
    }

    return true;
}
}  // namespace hestia