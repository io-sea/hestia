#include "HestiaCache.h"

#include <cstdlib>
#include <filesystem>

#include <iostream>

namespace hestia {
std::string HestiaCache::get_default_cache_dir()
{
    std::string cache_path;
    if (auto cache_path_env = std::getenv("HESTIA_CACHE_DIR");
        cache_path_env != nullptr) {
        cache_path = std::string(cache_path_env);
    }
#ifdef NDEBUG
    if (cache_path.empty()) {
        if (auto home_env = std::getenv("HOME"); home_env != nullptr) {
            cache_path = std::string(home_env) + "/.cache/hestia";
        }
    }
#endif

    if (cache_path.empty()) {
        cache_path = std::filesystem::current_path() / ".cache/hestia";
    }
    return cache_path;
}
}  // namespace hestia