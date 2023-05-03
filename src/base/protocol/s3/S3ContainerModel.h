#pragma once

#include "Metadata.h"
#include "Model.h"
#include "S3Container.h"

#include <unordered_map>
#include <vector>

namespace hestia {
class S3ContainerModel : public Model {
  public:
    static void deserialize(
        S3Container& container,
        const Metadata& metadata,
        const std::string& prefix);

    static Metadata::Query get_query_filter(const std::string& prefix);

    static Metadata serialize(
        const S3Container& container, const std::string& prefix);

    static std::string json_serialize(const S3Container& container);

  private:
    static bool is_internal_key(const std::string& key);
    static std::string without_prefix(
        const std::string& key, const std::string& prefix);

    static const std::vector<std::string> internal_keys;
    std::vector<S3Container> m_container;
};
}  // namespace hestia