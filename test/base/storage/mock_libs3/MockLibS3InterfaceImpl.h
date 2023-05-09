#pragma once

#include "IS3InterfaceImpl.h"
#include "MockLibS3.h"

#include <memory>

namespace hestia::mock {
class MockLibS3InterfaceImpl : public IS3InterfaceImpl {
  public:
    using Ptr = std::unique_ptr<MockLibS3InterfaceImpl>;
    static Ptr create();

    virtual ~MockLibS3InterfaceImpl() = default;

    void initialize(const S3Config& config) override;

    int put(const S3Object& obj, const Extent& extent, Stream* stream) override;

    int get(const S3Object& obj, const Extent& extent, Stream* stream) override;

    int remove(const S3Object& obj) override;

  private:
    libs3::LibS3 m_lib_s3;
};
}  // namespace hestia::mock