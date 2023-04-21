#pragma once

#include <string>
#include <vector>

struct MotrHsmTierInfo {
    std::string m_name;
    std::string m_identifier;
};

struct MotrConfig {
    std::string m_local_address;
    std::string m_ha_address;
    std::string m_profile;
    std::string m_proc_fid;
    std::string m_hsm_config_path;
    std::vector<MotrHsmTierInfo> m_tier_info;
};