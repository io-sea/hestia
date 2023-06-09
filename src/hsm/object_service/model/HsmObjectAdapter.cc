#include "HsmObjectAdapter.h"

#include "JsonUtils.h"
#include "StringUtils.h"

#include <iostream>

namespace hestia {

HsmObjectJsonAdapter::HsmObjectJsonAdapter(
    const std::string& internal_key_prefix) :
    m_internal_prefix(internal_key_prefix)
{
}

void HsmObjectJsonAdapter::to_string(
    const HsmObject& item, std::string& output) const
{
    Metadata data;
    data.set_item(m_internal_prefix + "::id", item.id());
    data.set_item(
        m_internal_prefix + "::last_modified",
        std::to_string(item.object().m_last_modified_time));
    data.set_item(
        m_internal_prefix + "::created",
        std::to_string(item.object().m_creation_time));
    data.set_item(
        m_internal_prefix + "::size", std::to_string(item.object().m_size));

    if (!item.tiers().empty()) {
        std::stringstream sstr;
        for (std::size_t idx = 0; idx < item.tiers().size() - 1; idx++) {
            sstr << std::to_string(item.tiers()[idx]) << ",";
        }
        if (!item.tiers().empty()) {
            sstr << std::to_string(item.tiers()[item.tiers().size() - 1]);
        }
        data.set_item(m_internal_prefix + "::tiers", sstr.str());
    }

    data.merge(item.object().m_metadata);

    output = JsonUtils::to_json(data);
}

void HsmObjectJsonAdapter::from_string(
    const std::string& input, HsmObject& item) const
{
    Metadata data;
    JsonUtils::from_json(input, data);

    auto on_item = [this,
                    &item](const std::string& key, const std::string& value) {
        const auto without_prefix =
            StringUtils::remove_prefix(key, m_internal_prefix + "::");
        if (without_prefix.size() == key.size()) {
            item.object().m_metadata.set_item(key, value);
        }
        else {
            if (without_prefix == "last_modified" && !value.empty()) {
                item.object().m_last_modified_time = std::stoull(value);
            }
            else if (without_prefix == "created" && !value.empty()) {
                item.object().m_creation_time = std::stoull(value);
            }
            else if (without_prefix == "size" && !value.empty()) {
                item.object().m_size = std::stoul(value);
            }
            else if (without_prefix == "tiers" && !value.empty()) {
                std::vector<std::string> tier_ids;
                StringUtils::split(value, ',', tier_ids);
                for (const auto& id : tier_ids) {
                    item.add_tier(std::stoi(id));
                }
            }
        }
    };
    data.for_each_item(on_item);
}

}  // namespace hestia