#include "S3UrlRouter.h"

#include "S3BucketListView.h"
#include "S3BucketView.h"
#include "S3ObjectView.h"

#include "S3Path.h"

#include <iostream>

namespace hestia {
void S3UrlRouter::set_object_view(std::unique_ptr<S3ObjectView> view)
{
    m_object_view = std::move(view);
}

void S3UrlRouter::set_bucket_view(std::unique_ptr<S3BucketView> view)
{
    m_bucket_view = std::move(view);
}

void S3UrlRouter::set_bucket_list_view(std::unique_ptr<S3BucketListView> view)
{
    m_bucket_list_view = std::move(view);
}

WebView* S3UrlRouter::get_view(const std::string& path)
{
    const auto s3_path = S3Path(path);
    if (!s3_path.m_object_key.empty()) {
        return m_object_view.get();
    }
    if (!s3_path.m_bucket_name.empty()) {
        return m_bucket_view.get();
    }
    return m_bucket_list_view.get();
}
}  // namespace hestia