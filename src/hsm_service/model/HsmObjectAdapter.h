#pragma once

#include "HsmObject.h"

#include <memory>
#include <sstream>

class HsmObjectAdapter {
  public:
    using Ptr = std::unique_ptr<HsmObjectAdapter>;

    static Ptr Create();

    void serialize(HsmObject& hsm_object);

    void parseTiers(HsmObject& hsm_object);

    void sync(const HsmObject& hsmObject, ostk::StorageObject& storageObject);

  private:
    void addIfNotEmpty(
        ostk::Metadata* md, const std::string& key, const std::string& value);

    static constexpr char LAST_MODIFIED_KEY[] = "last_modified";
    static constexpr char CREATED_KEY[]       = "creation_time";
    static constexpr char DATASET_KEY[]       = "dataset";
    static constexpr char TIERS_KEY[]         = "tiers";
};