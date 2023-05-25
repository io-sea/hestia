#include "UrlRouter.h"

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
        if (pattern.first == path) {
            return pattern.second.get();
        }
    }
    return nullptr;
}

}  // namespace hestia