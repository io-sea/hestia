#pragma once

#include <string>

namespace hestia {
class HestiaCache {
  public:
    static std::string get_default_cache_dir();
};
}  // namespace hestia