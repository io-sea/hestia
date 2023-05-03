#pragma once

#include "S3Container.h"
#include "S3Object.h"

#include "Extent.h"
#include "ReadableBufferView.h"
#include "WriteableBufferView.h"

#include <memory>

namespace hestia {
struct S3Config {
};

class IS3InterfaceImpl {
  public:
    using Ptr = std::unique_ptr<IS3InterfaceImpl>;

    virtual ~IS3InterfaceImpl() = default;

    virtual void initialize(const S3Config& config){};

    virtual int put(
        const S3Object& obj,
        const Extent& extent,
        const ReadableBufferView* buffer) = 0;

    virtual int get(
        const S3Object& obj,
        const Extent& extent,
        WriteableBufferView* buffer) = 0;

    virtual int remove(const S3Object& obj) = 0;
};
}  // namespace hestia