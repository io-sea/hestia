#include "UrlRouter.h"

#include "StringUtils.h"

namespace hestia {

void UrlRouter::add_pattern(
    const std::vector<std::string>& pattern, std::unique_ptr<WebView> view)
{
    m_views.push_back({pattern, std::move(view)});
}

WebView* UrlRouter::get_view(const std::string& path)
{
    for (const auto& patterns : m_views) {
        for (const auto& pattern : patterns.first) {
            if (StringUtils::starts_with(path, pattern)) {
                patterns.second->set_path(pattern);
                return patterns.second.get();
            }
        }
    }
    return nullptr;
}

}  // namespace hestia