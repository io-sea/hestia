#pragma once

#include "StorageObject.h"

#include <memory>
#include <string>
#include <vector>

namespace hestia {
class IPhobosInterfaceImpl {
  public:
    using Ptr = std::unique_ptr<IPhobosInterfaceImpl>;

    virtual ~IPhobosInterfaceImpl();

    virtual void init(){};

    virtual void finish(){};

    virtual std::string get(const StorageObject& obj, int fd) = 0;

    virtual void put(const StorageObject& obj, int fd) = 0;

    virtual bool exists(const StorageObject& obj) = 0;

    virtual void get_metadata(StorageObject& obj) = 0;

    virtual void remove(const StorageObject& object) = 0;

    virtual void list(
        const KeyValuePair& query, std::vector<StorageObject>& found) = 0;
};
}  // namespace hestia