#pragma once

#include "HsmObjectStoreClient.h"
#include "MotrConfig.h"

#include <memory>

namespace hestia {
class MotrInterface;

class MotrHsmClient : public HsmObjectStoreClient {
  public:
    MotrHsmClient();

    void intialize(const MotrConfig& config);

    virtual ~MotrHsmClient();

    void copy(const HsmObjectStoreRequest& request) const override;

    void get(
        const HsmObjectStoreRequest& request,
        StorageObject& object,
        Stream* stream) const override;

    void move(const HsmObjectStoreRequest& request) const override;

    void put(
        const HsmObjectStoreRequest& request, Stream* stream) const override;

    void remove(const HsmObjectStoreRequest& request) const override;

  protected:
    std::unique_ptr<MotrInterface> m_motr_interface;
};
}  // namespace hestia