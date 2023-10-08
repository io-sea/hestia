#include "S3BucketAdapter.h"

namespace hestia {
std::unique_ptr<S3BucketAdapter> S3BucketAdapter::create(const std::string&)
{
    return std::make_unique<S3BucketAdapter>();
}
}  // namespace hestia