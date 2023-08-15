#pragma once

#include "IS3InterfaceImpl.h"

struct S3BucketContext;

namespace hestia {

class LibS3InterfaceImpl : public IS3InterfaceImpl {
  public:
    ~LibS3InterfaceImpl();

    void initialize(const S3Config& config) override;

    int put(const S3Object& obj, const Extent& extent, Stream* stream) override;

    int get(const S3Object& obj, const Extent& extent, Stream* stream) override;

    int remove(const S3Object& obj) override;

  private:
    void get_bucket(
        const std::string& name, S3BucketContext* bucket_context) const;

    S3Config m_config;
};
}  // namespace hestia