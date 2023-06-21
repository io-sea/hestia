#pragma once

#include "WebView.h"

#include <memory>
#include <string>
#include <vector>

namespace hestia {
class UrlRouter {
  public:
    using Ptr = std::unique_ptr<UrlRouter>;
    using Pattern =
        std::pair<std::vector<std::string>, std::unique_ptr<WebView>>;

    virtual ~UrlRouter() = default;

    void add_pattern(
        const std::vector<std::string>& pattern, std::unique_ptr<WebView> view);

    virtual WebView* get_view(const std::string& path);

  private:
    std::vector<Pattern> m_views;
};
}  // namespace hestia