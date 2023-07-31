#pragma once

#include <string>

namespace hestia {
class HestiaCache {
  public:
    static std::string get_default_cache_dir();

  private:
    static std::string get_home_cache_dir();
    static constexpr char m_cache_loc[]{".cache/hestia"};
};
}  // namespace hestia