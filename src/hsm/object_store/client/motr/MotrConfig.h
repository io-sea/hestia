#pragma once

#include "Metadata.h"

#include <sstream>
#include <string>
#include <vector>

namespace hestia {

struct MotrHsmTierInfo {
    std::string m_name;
    std::string m_identifier;
    std::string m_description;
};

struct MotrConfig {

    std::string to_string() const;

    static std::string get_tier_info_for_m0hsm(
        const std::vector<MotrHsmTierInfo>& tier_info);

    void from_config(const Metadata& data);

    std::string m_local_address;
    std::string m_ha_address;
    std::string m_profile;
    std::string m_proc_fid;
    std::string m_hsm_config_path;
    std::vector<MotrHsmTierInfo> m_tier_info;
};

}  // namespace hestia