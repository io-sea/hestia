#pragma once

#include "HsmObject.h"

#include <memory>
#include <sstream>

namespace hestia {
class HsmObjectAdapter {
  public:
    using Ptr = std::unique_ptr<HsmObjectAdapter>;

    static Ptr create();

    void serialize(HsmObject& hsm_object);

    void parse_tiers(HsmObject& hsm_object);

    void sync(
        const HsmObject& hsm_object, hestia::StorageObject& storage_object);

  private:
    void add_if_not_empty(
        hestia::Metadata* md, const std::string& key, const std::string& value);

    static constexpr char last_modified_key[] = "last_modified";
    static constexpr char created_key[]       = "creation_time";
    static constexpr char dataset_key[]       = "dataset";
    static constexpr char tiers_key[]         = "tiers";
};
}  // namespace hestia