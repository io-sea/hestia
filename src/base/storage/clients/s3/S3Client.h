#pragma once

#include "IS3InterfaceImpl.h"
#include "ObjectStoreClient.h"

#include <memory>

namespace hestia {

class S3ContainerAdapter;
class S3ObjectAdapter;

class S3Client : public ObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<S3Client>;

    S3Client(IS3InterfaceImpl::Ptr impl = nullptr);

    virtual ~S3Client() = default;

    static Ptr create(IS3InterfaceImpl::Ptr impl = nullptr);

    static Ptr create(
        const S3Config& config, IS3InterfaceImpl::Ptr impl = nullptr);

    static std::string get_registry_identifier();

    void initialize(const Dictionary& config) override;

    void do_initialize(const S3Config& config);

  private:
    bool exists(const StorageObject& object) const override;

    void put(const StorageObject& object, const Extent& extent, Stream* stream)
        const override;

    void get(StorageObject& object, const Extent& extent, Stream* stream)
        const override;

    void remove(const StorageObject& obj) const override;

    void list(const Metadata::Query& query, std::vector<StorageObject>& found)
        const override;

    IS3InterfaceImpl::Ptr m_impl;
    std::unique_ptr<S3ContainerAdapter> m_container_adapter;
    std::unique_ptr<S3ObjectAdapter> m_object_adapter;
};
}  // namespace hestia