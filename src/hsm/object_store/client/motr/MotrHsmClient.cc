#include "MotrHsmClient.h"

#include "MotrInterface.h"
#include "StringUtils.h"

namespace hestia {
MotrHsmClient::MotrHsmClient(std::unique_ptr<MotrInterface> interface) :
    m_motr_interface(std::move(interface))
{
    if (m_motr_interface == nullptr) {
        m_motr_interface = std::make_unique<MotrInterface>();
    }
}

void MotrHsmClient::initialize(const Metadata& data)
{
    MotrConfig config;

    auto on_each_item =
        [&config](const std::string& key, const std::string& value) {
            if (key == "local_address") {
                config.m_local_address = value;
            }
            else if (key == "ha_address") {
                config.m_ha_address = value;
            }
            else if (key == "profile") {
                config.m_profile = value;
            }
            else if (key == "proc_fid") {
                config.m_proc_fid = value;
            }
            else if (key == "hsm_config_path") {
                config.m_hsm_config_path = value;
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
                    config.m_tier_info.push_back(tier_info);
                }
            }
        };
    data.for_each_item(on_each_item);
    do_intialize(config);
}

void MotrHsmClient::do_intialize(const MotrConfig& config)
{
    m_motr_interface->initialize(config);
}

void MotrHsmClient::copy(const HsmObjectStoreRequest& request) const
{
    m_motr_interface->copy(request);
}

void MotrHsmClient::get(
    const HsmObjectStoreRequest& request,
    StorageObject& object,
    Stream* stream) const
{
    m_motr_interface->get(request, object, stream);
}

void MotrHsmClient::move(const HsmObjectStoreRequest& request) const
{
    m_motr_interface->move(request);
}

void MotrHsmClient::put(
    const HsmObjectStoreRequest& request, Stream* stream) const
{
    m_motr_interface->put(request, stream);
}

void MotrHsmClient::remove(const HsmObjectStoreRequest& request) const
{
    m_motr_interface->remove(request);
}
}  // namespace hestia