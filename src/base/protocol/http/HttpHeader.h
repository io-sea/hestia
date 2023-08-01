#pragma once

#include "Map.h"

#include <vector>

namespace hestia {
class HttpHeader {
  public:
    HttpHeader();

    HttpHeader(const std::vector<std::string>& lines);

    void for_each(Map::onItem func) const;

    std::string get_item(const std::string& key) const;

    const Map& get_data() const;

    Map get_items_with_prefix(const std::string& prefix) const;

    std::string get_content_length() const;

    void set_item(const std::string& key, const std::string& value);

    void set_items(const Map& items);

    std::string to_string() const;

    bool has_html_accept_type() const;

    void set_content_type(const std::string& content_type);

    void set_auth_token(const std::string& token);

  private:
    void parse_accept_types(const std::string& header_value);

    std::string m_http_version{"1.1"};
    std::string m_content_type{"text/plain"};
    std::vector<std::string> m_accept_types;
    Map m_data;
};
}  // namespace hestia