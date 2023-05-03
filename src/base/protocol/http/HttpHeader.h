#pragma once

#include "Metadata.h"

#include <vector>

namespace hestia {
class HttpHeader {
  public:
    HttpHeader() = default;

    HttpHeader(const std::vector<std::string>& lines);

    void for_each(Metadata::onItem func) const;

    std::string get_item(const std::string& key) const;

    Metadata get_items_with_prefix(const std::string& prefix) const;

    std::string get_content_length() const;

    void set_item(const std::string& key, const std::string& value);

    std::string to_string() const;

  private:
    std::string m_http_version{"1.1"};
    std::string m_content_type{"text / html"};
    Metadata m_data;
};
}  // namespace hestia