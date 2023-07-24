#pragma once

#include "SerializeableWithFields.h"

#include "S3Container.h"
#include "S3Object.h"

#include "Extent.h"
#include "Stream.h"

#include <memory>

namespace hestia {
class S3Config : public SerializeableWithFields {

  public:
    S3Config() : SerializeableWithFields("s3_object_store_client_config")
    {
        register_scalar_field(&m_metadataprefix);
        register_scalar_field(&m_user_agent);
        register_scalar_field(&m_default_host);
    }

    StringField m_metadataprefix{"metadata_prefix"};
    StringField m_user_agent{"user_agent"};
    StringField m_default_host{"default_host"};
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