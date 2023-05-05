#pragma once

#include <string>

namespace hestia {
class HestiaCli {
  public:
    enum class Method { PUT, GET, COPY, MOVE, RELEASE, UNKNOWN };

    void parse(int argc, char* argv[]);

    Method m_method{Method::UNKNOWN};
    std::string m_object_id;
    std::string m_source_tier;
    std::string m_target_tier;
    std::string m_path;
    std::string m_config_path;
};
}  // namespace hestia