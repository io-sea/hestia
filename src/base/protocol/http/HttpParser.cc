#include "HttpParser.h"

#include "HttpParser.h"
#include "StringUtils.h"

namespace hestia {
bool HttpParser::parse_form_data(const std::string& input, Metadata& parsed)
{
    std::vector<std::string> elements;
    StringUtils::split(input, '&', elements);
    for (const auto& element : elements) {
        const auto& [key, value] = StringUtils::split_on_first(element, '=');
        parsed.set_item(key, value);
    }
    return true;
}

bool HttpParser::parse_preamble(const std::string& line, HttpPreamble& preamble)
{
    bool in_path{false};
    bool in_query{false};
    bool in_method{true};
    bool in_protocol{false};

    std::string query_string;

    for (const auto c : line) {
        if (in_path) {
            if (std::isspace(c) != 0) {
                in_path     = false;
                in_protocol = true;
            }
            else if (c == '?') {
                in_path  = false;
                in_query = true;
            }
            else {
                preamble.m_path.push_back(c);
            }
        }
        else if (in_query) {
            if (std::isspace(c) != 0) {
                in_query    = false;
                in_protocol = true;
            }
            else {
                query_string.push_back(c);
            }
        }
        else if (in_method) {
            if (std::isspace(c) != 0) {
                in_method = false;
                in_path   = true;
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

    if (!query_string.empty()) {
        std::vector<std::string> elements;
        StringUtils::split(query_string, '&', elements);
        for (const auto& element : elements) {
            const auto& [key, value] =
                StringUtils::split_on_first(element, '=');
            preamble.m_queries.set_item(key, value);
        }
    }
    return true;
}
}  // namespace hestia