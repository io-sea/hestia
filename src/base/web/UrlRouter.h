#pragma once

#include "WebView.h"

#include <memory>
#include <string>
#include <vector>

namespace hestia {
class UrlRouter {
  public:
    using Ptr     = std::unique_ptr<UrlRouter>;
    using Pattern = std::pair<std::string, std::unique_ptr<WebView>>;

    virtual ~UrlRouter() = default;

    void add_pattern(const std::string& pattern, std::unique_ptr<WebView> view)
    {
        m_views.push_back({pattern, std::move(view)});
    }

    virtual WebView* get_view(const std::string& path)
    {
        for (auto& pattern : m_views) {
            if (pattern.first == path) {
                return pattern.second.get();
            }
        }
        return nullptr;
    }

  private:
    std::vector<Pattern> m_views;
};
}  // namespace hestia