#include "LibS3InterfaceImpl.h"

#include <libs3.h>
#include <stdexcept>

namespace hestia {
void LibS3InterfaceImpl::initialize(const S3Config& config)
{
    auto status = S3_initialize(nullptr, 0, nullptr);
    if (status != S3Status::S3StatusOK) {
        throw std::runtime_error("Failed to initialize libs3");
    }
}

int LibS3InterfaceImpl::put(
    const S3Object& obj, const Extent& extent, const ReadableBufferView* buffer)
{
    return 0;
}

int LibS3InterfaceImpl::get(
    const S3Object& obj, const Extent& extent, WriteableBufferView* buffer)
{
    return 0;
}

int LibS3InterfaceImpl::remove(const S3Object& obj)
{
    return 0;
}
}  // namespace hestia
