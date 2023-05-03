#pragma once

#include <sstream>

namespace hestia {
class XmlUtils {
  public:
    static void add_default_prolog(std::stringstream& sstr);

    static void add_tag_and_text(
        std::stringstream& sstr,
        const std::string& tag,
        const std::string& text);

    static void close_tag(std::stringstream& sstr, const std::string& tag);

    static void open_tag(std::stringstream& sstr, const std::string& tag);
};
}  // namespace hestia