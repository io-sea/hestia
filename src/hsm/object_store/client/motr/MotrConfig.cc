#include "MotrConfig.h"

#include "StringUtils.h"

namespace hestia {
std::string MotrConfig::to_string() const
{
    std::stringstream sstr;
    sstr << "[local address: " << m_local_address << "; ";
    sstr << "ha address: " << m_ha_address << "; ";
    sstr << "profile: " << m_profile << "; ";
    sstr << "proc_fid: " << m_proc_fid << "; ";
    sstr << "hsm_config_path: " << m_hsm_config_path << "]";
    return sstr.str();
}

std::string MotrConfig::get_tier_info_for_m0hsm(
    const std::vector<MotrHsmTierInfo>& tier_info)
{
    std::stringstream sstr;
    for (const auto& tier : tier_info) {
        sstr << tier.m_name << " = " << tier.m_identifier << "\n";
    }
    return sstr.str();
}

void MotrConfig::from_config(const Metadata& data)
{
    auto on_each_item =
        [this](const std::string& key, const std::string& value) {
            if (key == "local_address") {
                m_local_address = value;
            }
            else if (key == "ha_address") {
                m_ha_address = value;
            }
            else if (key == "profile") {
                m_profile = value;
            }
            else if (key == "proc_fid") {
                m_proc_fid = value;
            }
            else if (key == "hsm_config_path") {
                m_hsm_config_path = value;
            }
            else if (key == "tier_info") {
                std::vector<std::string> entries;
                StringUtils::split(value, ';', entries);
                for (const auto& entry : entries) {
                    std::vector<std::string> args;
                    StringUtils::split(entry, ',', args);

                    MotrHsmTierInfo tier_info;
                    for (const auto& arg : args) {
                        const auto& [key, value] =
                            StringUtils::split_on_first(arg, '=');
                        if (key == "name") {
                            tier_info.m_name = value;
                        }
                        else if (key == "identifier") {
                            tier_info.m_identifier = value;
                        }
                    }
                    m_tier_info.push_back(tier_info);
                }
            }
        };
    data.for_each_item(on_each_item);
}
}  // namespace hestia