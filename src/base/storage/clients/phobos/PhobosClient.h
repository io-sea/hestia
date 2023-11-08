#pragma once

#include "ObjectStoreClient.h"

#include "PhobosInterface.h"

namespace hestia {
class PhobosClient : public ObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<PhobosClient>;

    PhobosClient(PhobosInterface::Ptr phobos_interface = nullptr);

    virtual ~PhobosClient();

    static Ptr create(PhobosInterface::Ptr phobos_interface = nullptr);

    static std::string get_registry_identifier();

  private:
    void get(
        const ObjectStoreRequest& request,
        completionFunc completion_func,
        Stream* stream                     = nullptr,
        Stream::progressFunc progress_func = nullptr) const override;

    bool exists(const StorageObject& object) const override;

    void put(
        const ObjectStoreRequest& request,
        completionFunc completion_func,
        Stream* stream                     = nullptr,
        Stream::progressFunc progress_func = nullptr) const override;

    void remove(const StorageObject& object) const override;

    void list(const KeyValuePair& query, std::vector<StorageObject>& found)
        const override;

    std::unique_ptr<PhobosInterface> m_phobos_interface;
};
}  // namespace hestia
