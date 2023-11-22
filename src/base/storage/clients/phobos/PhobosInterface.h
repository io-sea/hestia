#pragma once

#include "StorageObject.h"

#include <memory>
#include <vector>

namespace hestia {
class IPhobosInterfaceImpl;

class PhobosInterface {
  public:
    using Ptr = std::unique_ptr<PhobosInterface>;

    PhobosInterface(std::unique_ptr<IPhobosInterfaceImpl> impl = nullptr);

    ~PhobosInterface();

    static PhobosInterface::Ptr create(
        std::unique_ptr<IPhobosInterfaceImpl> impl = nullptr);

    void init();

    void finish();

    void get(const StorageObject& object, int fd);

    void put(const StorageObject& object, int fd);

    void get_metadata(StorageObject& object);

    bool exists(const StorageObject& obj);

    void remove(const StorageObject& object);

    void list(const KeyValuePair& query, std::vector<StorageObject>& found);

  private:
    std::unique_ptr<IPhobosInterfaceImpl> m_impl;
};
}  // namespace hestia