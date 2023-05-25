#pragma once

#include "IPhobosInterfaceImpl.h"
#include "MockPhobos.h"

namespace hestia::mock {
class MockPhobosInterface : public IPhobosInterfaceImpl {
  public:
    MockPhobosInterface();

    static std::unique_ptr<MockPhobosInterface> create();

    void get(const StorageObject& obj, int fd) override;

    bool exists(const StorageObject& obj) override;

    void get_metadata(StorageObject& obj) override;

    void list(const Metadata::Query& query, std::vector<StorageObject>& found)
        override;

    void put(const StorageObject& obj, int fd) override;

    void remove(const StorageObject& object) override;

  private:
    MockPhobos m_phobos;
};
}  // namespace hestia::mock