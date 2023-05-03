#include "XmlUtils.h"

namespace hestia {
void XmlUtils::add_default_prolog(std::stringstream& sstr)
{
    sstr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
}

void XmlUtils::close_tag(std::stringstream& sstr, const std::string& tag)
{
    sstr << "</" + tag + ">\n";
}

void XmlUtils::open_tag(std::stringstream& sstr, const std::string& tag)
{
    sstr << "<" + tag + ">\n";
}

void XmlUtils::add_tag_and_text(
    std::stringstream& sstr, const std::string& tag, const std::string& text)
{
    sstr << "<" << tag << ">" << text << "</" << tag << ">\n";
}
}  // namespace hestia