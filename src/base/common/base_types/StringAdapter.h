#pragma once

#include <string>

namespace hestia {
template<typename ItemT>
class StringAdapter {
  public:
    virtual ~StringAdapter() = default;

    virtual void to_string(const ItemT& item, std::string& output) const = 0;

    virtual void from_string(const std::string& output, ItemT& item) const = 0;
};
}  // namespace hestia