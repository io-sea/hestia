#pragma once

#include "SerializeableWithFields.h"

#include <sstream>
#include <string>
#include <vector>

namespace hestia {

class MotrHsmTierInfo : public SerializeableWithFields {

  public:
    MotrHsmTierInfo() : SerializeableWithFields("motr_hsm_tier_info")
    {
        register_scalar_field(&m_name);
        register_scalar_field(&m_identifier);
        register_scalar_field(&m_description);
    }

    void set_identifier(const std::string& id)
    {
        m_identifier.update_value(id);
    }

    StringField m_name{"name"};
    StringField m_identifier{"identifier"};
    StringField m_description{"description"};
};

class MotrConfig : public SerializeableWithFields {
  public:
    MotrConfig();

    std::string to_string() const;

    static std::string get_tier_info_for_m0hsm(
        const std::vector<MotrHsmTierInfo>& tier_info);

    void add_tier_info(const MotrHsmTierInfo& tier_info)
    {
        m_tier_info.get_container_as_writeable().push_back(tier_info);
    }

    StringField m_local_address{"local_address"};
    StringField m_ha_address{"ha_address"};
    StringField m_profile{"profile"};
    StringField m_proc_fid{"proc_fid"};
    StringField m_hsm_config_path{"hsm_config_path"};

    SequenceField<std::vector<MotrHsmTierInfo>> m_tier_info{"tier_info"};
};

}  // namespace hestia