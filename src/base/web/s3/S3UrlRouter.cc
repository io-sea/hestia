#include "S3UrlRouter.h"

#include "S3ContainerListView.h"
#include "S3ContainerView.h"
#include "S3ObjectView.h"

#include "S3Path.h"

#include <iostream>

namespace hestia {
void S3UrlRouter::set_object_view(std::unique_ptr<S3ObjectView> view)
{
    m_object_view = std::move(view);
}

void S3UrlRouter::set_container_view(std::unique_ptr<S3ContainerView> view)
{
    m_container_view = std::move(view);
}

void S3UrlRouter::set_container_list_view(
    std::unique_ptr<S3ContainerListView> view)
{
    m_container_list_view = std::move(view);
}

WebView* S3UrlRouter::get_view(const std::string& path)
{
    const auto s3_path = S3Path(path);
    if (!s3_path.m_object_id.empty()) {
        return m_object_view.get();
    }
    if (!s3_path.m_container_name.empty()) {
        return m_container_view.get();
    }
    return m_container_list_view.get();
}
}  // namespace hestia