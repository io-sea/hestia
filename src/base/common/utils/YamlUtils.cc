#include "YamlUtils.h"

#include "yaml-cpp/yaml.h"

#include <iostream>

namespace hestia {
void YamlUtils::load(const std::string& path, NestedMetadata& data)
{
    (void)data;

    YAML::Node config = YAML::LoadFile(path);
    if (config.IsNull()) {
        return;
    }

    if (config.IsSequence()) {
        for (std::size_t idx = 0; idx < config.size(); idx++) {
            if (config[idx].IsMap()) {
                for (const auto& kv_pair : config[idx]) {
                    data.set_item(
                        kv_pair.first.as<std::string>(),
                        kv_pair.second.as<std::string>());
                }
            }
        }
    }
}
}  // namespace hestia
