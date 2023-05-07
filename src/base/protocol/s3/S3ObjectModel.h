#pragma once

#include "Metadata.h"
#include "Model.h"

#include "S3Container.h"
#include "S3Object.h"

#include <array>
#include <vector>

namespace hestia {
class S3ObjectModel : public Model {
  public:
    static void deserialize(
        S3Object& object, const Metadata& metadata, const std::string& prefix);

    static std::string get_query_filter(const std::string& prefix);

    static Metadata serialize(
        const S3Container& container,
        const S3Object& object,
        const std::string& prefix);

    static std::string json_serialize(
        const S3Container& container, const S3Object& object);

  private:
    static constexpr char query_filter[] = "bucket";

    static bool is_internal_key(const std::string& key);
    static std::string without_prefix(
        const std::string& key, const std::string& prefix);
    static const std::array<std::string, 5> internal_keys;

    std::vector<S3Object> m_objects;
};
}  // namespace hestia