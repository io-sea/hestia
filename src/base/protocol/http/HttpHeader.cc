#include "HttpHeader.h"

#include "StringUtils.h"

#include <sstream>

namespace hestia {
HttpHeader::HttpHeader(const std::vector<std::string>& lines)
{
    for (const auto& line : lines) {
        const auto loc = line.find(":");
        if (loc == line.npos) {
            continue;
        }

        const auto tag   = line.substr(0, loc);
        const auto value = line.substr(loc + 1, line.size() - loc);

        if (tag.empty() || value.empty()) {
            continue;
        }
        set_item(tag, value);
    }
}

std::string HttpHeader::get_item(const std::string& key) const
{
    return m_data.get_item(StringUtils::to_lower(key));
}

void HttpHeader::for_each(Metadata::onItem func) const
{
    m_data.for_each_item(func);
}

void HttpHeader::set_item(const std::string& key, const std::string& value)
{
    m_data.set_item(StringUtils::to_lower(key), value);
}

std::string HttpHeader::to_string() const
{
    std::stringstream sstr;
    auto each_item =
        [&sstr](const std::string& header, const std::string& val) {
            sstr << header << ":" << val << "\n";
        };
    m_data.for_each_item(each_item);
    return sstr.str();
}

Metadata HttpHeader::get_items_with_prefix(const std::string& prefix) const
{
    Metadata metadata;
    auto on_header_item =
        [&metadata, prefix](const std::string& header, const std::string& val) {
            if (header.find(prefix) != std::string::npos) {
                std::string key =
                    header.substr(prefix.size(), header.size() - 1);
                metadata.set_item(key, val);
            }
        };
    m_data.for_each_item(on_header_item);
    return metadata;
}

std::string HttpHeader::get_content_length() const
{
    return get_item("Content-Length");
}
}  // namespace hestia