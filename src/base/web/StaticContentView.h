#pragma once

#include "WebView.h"

#include <filesystem>
#include <unordered_map>

namespace hestia {
class StaticContentView : public WebView {
  public:
    StaticContentView(const std::string& directory, bool cache = true);

  private:
    HttpResponse::Ptr on_get(const HttpRequest& request) override;

    bool read(const std::string& path, std::string& content) const;

    std::filesystem::path m_directory;
    bool m_should_cache{true};
    std::unordered_map<std::string, std::string> m_cache;
};
}  // namespace hestia