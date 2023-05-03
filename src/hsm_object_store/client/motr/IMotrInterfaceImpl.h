#pragma once

#include "HsmObjectStoreRequest.h"
#include "MotrConfig.h"

#include "Stream.h"

#include <string>
#include <vector>

namespace hestia {
class IMotrInterfaceImpl {
  public:
    virtual ~IMotrInterfaceImpl()                     = default;
    virtual void initialize(const MotrConfig& config) = 0;

    virtual void copy(const HsmObjectStoreRequest& request) const = 0;

    virtual void get(
        const HsmObjectStoreRequest& request,
        StorageObject& object,
        Stream* stream) const = 0;

    virtual void move(const HsmObjectStoreRequest& request) const = 0;

    virtual void put(
        const HsmObjectStoreRequest& request, Stream* stream) const = 0;

    virtual void remove(const HsmObjectStoreRequest& request) const = 0;

    static constexpr std::size_t max_obj_length{0xffffffffffffffff};

  protected:
    virtual void initialize_hsm(const std::vector<MotrHsmTierInfo>&) = 0;
};
}  // namespace hestia