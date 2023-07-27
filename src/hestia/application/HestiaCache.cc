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

    const std::string home_cache_path = get_home_cache_dir();

    // In Release use the home directory - but in Debug try to use the cwd
    // (which can be empty for child procs etc)
#ifdef NDEBUG
    if (cache_path.empty()) {
        cache_path = home_cache_path;
    }
#endif

    if (cache_path.empty()) {
        if (const auto cwd = std::filesystem::current_path();
            cwd.empty() || cwd == "/") {
            cache_path = home_cache_path;
        }
        else {
            cwd / m_cache_loc;
        }
    }
    return cache_path;
}

std::string HestiaCache::get_home_cache_dir()
{
    if (auto home_env = std::getenv("HOME"); home_env != nullptr) {
        return std::string(home_env) + "/" + m_cache_loc;
    }
    return {};
}
}  // namespace hestia