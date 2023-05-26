#pragma once

#include "StorageTier.h"
#include "StringAdapter.h"

namespace hestia {
class StorageTierJsonAdapter : public StringAdapter<StorageTier> {
  public:
    void to_string(const StorageTier& item, std::string& output) const override;

    void from_string(
        const std::string& output, StorageTier& item) const override;
};

}  // namespace hestia