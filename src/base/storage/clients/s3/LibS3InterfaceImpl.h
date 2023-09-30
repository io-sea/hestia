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
    bool test_bucket(const std::string& name, const S3Object& obj) const;

    void create_bucket(const std::string& name, const S3Object& obj) const;

    std::string get_bucket_name(const S3Object& obj) const;

    void prepare_bucket_context(
        S3BucketContext& bucket_context,
        const std::string& name,
        const std::string& access_id,
        const std::string& access_token,
        const S3Object& obj) const;

    void validate_bucket_name(const std::string& name) const;

    S3Config m_config;
};
}  // namespace hestia