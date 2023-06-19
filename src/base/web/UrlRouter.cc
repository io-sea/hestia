#include "UrlRouter.h"

#include "StringUtils.h"

namespace hestia {

void UrlRouter::add_pattern(
    const std::string& pattern, std::unique_ptr<WebView> view)
{
    view->set_path(pattern);
    m_views.push_back({pattern, std::move(view)});
}

WebView* UrlRouter::get_view(const std::string& path)
{
    for (auto& pattern : m_views) {
        if (StringUtils::starts_with(path, pattern.first)) {
            return pattern.second.get();
        }
    }
    return nullptr;
}

}  // namespace hestia