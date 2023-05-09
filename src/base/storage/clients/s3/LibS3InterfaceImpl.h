#pragma once

#include "IS3InterfaceImpl.h"

namespace hestia {
class LibS3InterfaceImpl : public IS3InterfaceImpl {
  public:
    void initialize(const S3Config& config) override;

    int put(const S3Object& obj, const Extent& extent, Stream* stream) override;

    int get(const S3Object& obj, const Extent& extent, Stream* stream) override;

    int remove(const S3Object& obj) override;
};
}  // namespace hestia