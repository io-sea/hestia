#pragma once

#include "IPhobosInterfaceImpl.h"
#include "MockPhobos.h"

namespace hestia::mock {
class MockPhobosInterface : public IPhobosInterfaceImpl {
  public:
    MockPhobosInterface();

    static std::unique_ptr<MockPhobosInterface> create();

    std::string get(const StorageObject& obj, int fd) override;

    bool exists(const StorageObject& obj) override;

    void get_metadata(StorageObject& obj) override;

    void list(
        const KeyValuePair& query, std::vector<StorageObject>& found) override;

    void put(const StorageObject& obj, int fd) override;

    void remove(const StorageObject& object) override;

    void set_redirect_location(const std::string& location);

  private:
    std::string m_redirect_location;
    MockPhobos m_phobos;
};
}  // namespace hestia::mock