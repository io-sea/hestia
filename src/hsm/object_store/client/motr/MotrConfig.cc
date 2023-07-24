#include "MotrConfig.h"

#include "StringUtils.h"

namespace hestia {

MotrConfig::MotrConfig() :
    SerializeableWithFields("motr_hsm_object_store_client_config")
{
    register_scalar_field(&m_local_address);
    register_scalar_field(&m_ha_address);
    register_scalar_field(&m_profile);
    register_scalar_field(&m_proc_fid);
    register_scalar_field(&m_hsm_config_path);
    register_sequence_field(&m_tier_info);
}

std::string MotrConfig::to_string() const
{
    std::stringstream sstr;
    sstr << "[local address: " << m_local_address.get_value() << "; ";
    sstr << "ha address: " << m_ha_address.get_value() << "; ";
    sstr << "profile: " << m_profile.get_value() << "; ";
    sstr << "proc_fid: " << m_proc_fid.get_value() << "; ";
    sstr << "hsm_config_path: " << m_hsm_config_path.get_value() << "]";
    return sstr.str();
}

std::string MotrConfig::get_tier_info_for_m0hsm(
    const std::vector<MotrHsmTierInfo>& tier_info)
{
    std::stringstream sstr;
    for (const auto& tier : tier_info) {
        sstr << tier.m_name.get_value() << " = "
             << tier.m_identifier.get_value() << "\n";
    }
    return sstr.str();
}

}  // namespace hestia