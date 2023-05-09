#pragma once

#include "S3Container.h"
#include "S3Object.h"

#include "Extent.h"
#include "Stream.h"

#include <memory>

namespace hestia {
struct S3Config {
    std::string m_metadataprefix;
    std::string m_user_agent;
    std::string m_default_host;
};

class IS3InterfaceImpl {
  public:
    using Ptr = std::unique_ptr<IS3InterfaceImpl>;

    virtual ~IS3InterfaceImpl() = default;

    virtual void initialize(const S3Config& config) = 0;

    virtual int put(
        const S3Object& obj, const Extent& extent, Stream* stream) = 0;

    virtual int get(
        const S3Object& obj, const Extent& extent, Stream* stream) = 0;

    virtual int remove(const S3Object& obj) = 0;
};
}  // namespace hestia