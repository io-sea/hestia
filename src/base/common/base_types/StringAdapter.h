#pragma once

#include "Metadata.h"
#include <string>

namespace hestia {
template<typename ItemT>
class StringAdapter {
  public:
    virtual ~StringAdapter() = default;

    virtual void to_string(
        const ItemT& item,
        std::string& output,
        const std::string& id = {}) const = 0;

    virtual void to_string(
        const std::vector<ItemT>& items, std::string& output) const = 0;

    virtual void from_string(const std::string& output, ItemT& item) const = 0;

    virtual void from_string(
        const std::string& output, std::vector<ItemT>& item) const = 0;

    virtual bool matches_query(
        const ItemT& item, const Metadata& query) const = 0;
};
}  // namespace hestia