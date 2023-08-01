#pragma once

#include "WebView.h"

#include <filesystem>
#include <unordered_map>

namespace hestia {
class StaticContentView : public WebView {
  public:
    enum class Type { DISK, BUFFER };

    StaticContentView(const std::string& directory = {}, bool cache = true);

    void set_buffer(const std::string& buffer);

  private:
    HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user) override;

    bool read(const std::string& path, std::string& content) const;

    std::filesystem::path m_directory;
    bool m_should_cache{true};
    std::string m_buffer;
    Type m_type{Type::DISK};
    std::unordered_map<std::string, std::string> m_cache;
};
}  // namespace hestia