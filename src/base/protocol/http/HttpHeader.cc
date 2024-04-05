#include "HttpHeader.h"

#include "StringUtils.h"

#include <sstream>

namespace hestia {

HttpHeader::HttpHeader()
{
    // set_item("Content-Type", m_content_type);
}

HttpHeader::HttpHeader(const std::vector<std::string>& lines)
{
    for (const auto& line : lines) {
        add_line(line);
    }
}

void HttpHeader::add_line(const std::string& line)
{
    const auto loc = line.find(":");
    if (loc == line.npos) {
        return;
    }

    auto tag = line.substr(0, loc);
    StringUtils::trim(tag);
    auto value = line.substr(loc + 1, line.size() - loc);
    StringUtils::trim(value);

    if (tag.empty() || value.empty()) {
        return;
    }
    set_item(tag, value);
}

void HttpHeader::set_items(const Map& items)
{
    m_data.merge(items);
}

const Map& HttpHeader::get_data() const
{
    return m_data;
}

std::string HttpHeader::get_item(const std::string& key) const
{
    return m_data.get_item(StringUtils::to_lower(key));
}

void HttpHeader::for_each(Map::onItem func) const
{
    m_data.for_each_item(func);
}

void HttpHeader::set_item(const std::string& key, const std::string& value)
{
    m_data.set_item(StringUtils::to_lower(key), value);

    if (StringUtils::to_lower(key) == "accept") {
        parse_accept_types(value);
    }
}

void HttpHeader::set_auth_token(const std::string& token)
{
    set_item("Authorization", token);
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

Map HttpHeader::get_items_with_prefix(const std::string& prefix) const
{
    Map metadata;
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

void HttpHeader::parse_accept_types(const std::string& header_value)
{
    std::vector<std::string> comma_splits;
    StringUtils::split(header_value, ',', comma_splits);

    for (auto entry : comma_splits) {
        StringUtils::trim(entry);
        m_accept_types.push_back(StringUtils::to_lower(entry));
    }
}

bool HttpHeader::has_html_accept_type() const
{
    for (const auto& mime_type : m_accept_types) {
        if (StringUtils::starts_with(mime_type, "text/html")) {
            return true;
        }
    }
    return false;
}

bool HttpHeader::has_expect_continue() const
{
    return get_item("Expect") == "100-continue";
}

void HttpHeader::set_content_type(const std::string& content_type)
{
    m_content_type = content_type;
    set_item("Content-Type", m_content_type);
}

std::string HttpHeader::get_content_length() const
{
    return get_item("Content-Length");
}

std::size_t HttpHeader::get_content_length_as_size_t() const
{
    const auto length_str = get_content_length();
    if (length_str.empty()) {
        return 0;
    }

    try {
        auto length = std::stoull(length_str);
        return length;
    }
    catch (const std::exception&) {
        return 0;
    }
}

}  // namespace hestia