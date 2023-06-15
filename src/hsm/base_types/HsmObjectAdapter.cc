#include "HsmObjectAdapter.h"

#include "JsonUtils.h"
#include "StringUtils.h"
#include "Uuid.h"

#include <iostream>

namespace hestia {

HsmObjectJsonAdapter::HsmObjectJsonAdapter(
    const std::string& internal_key_prefix) :
    m_internal_prefix(internal_key_prefix)
{
}

void HsmObjectJsonAdapter::to_string(
    const std::vector<HsmObject>& items, std::string& output) const
{
    auto seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& item : items) {
        auto dict = std::make_unique<Dictionary>();
        to_dict(item, *dict);
        seq->add_sequence_item(std::move(dict));
    }
    output = JsonUtils::to_json(*seq);
}

void HsmObjectJsonAdapter::to_dict(
    const HsmObject& item, Dictionary& dict, const std::string& id) const
{
    std::string working_id;
    if (!id.empty()) {
        working_id = id;
    }
    else {
        working_id = item.id();
    }

    std::unordered_map<std::string, std::string> data{
        {m_internal_prefix + "::id", working_id},
        {m_internal_prefix + "::last_modified",
         std::to_string(item.object().m_last_modified_time)},
        {m_internal_prefix + "::created",
         std::to_string(item.object().m_creation_time)},
        {m_internal_prefix + "::size", std::to_string(item.object().m_size)},
    };
    if (!item.tiers().empty()) {
        std::stringstream sstr;
        for (std::size_t idx = 0; idx < item.tiers().size() - 1; idx++) {
            sstr << std::to_string(item.tiers()[idx]) << ",";
        }
        if (!item.tiers().empty()) {
            sstr << std::to_string(item.tiers()[item.tiers().size() - 1]);
        }
        data[m_internal_prefix + "::tiers"] = sstr.str();
    }
    else {
        data[m_internal_prefix + "::tiers"] = "";
    }
    data.insert(
        item.object().m_metadata.get_raw_data().begin(),
        item.object().m_metadata.get_raw_data().end());
    dict.set_map(data);
}

void HsmObjectJsonAdapter::from_dict(
    const Dictionary& dict, HsmObject& item) const
{
    Metadata scalar_data;
    dict.get_map_items(scalar_data);

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
            else if (without_prefix == "id" && !value.empty()) {
                if (item.object().m_id.empty()) {
                    item.object().m_id = value;
                }
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
    scalar_data.for_each_item(on_item);
}

void HsmObjectJsonAdapter::to_string(
    const HsmObject& item, std::string& output, const std::string& id) const
{
    Dictionary dict;
    to_dict(item, dict, id);
    output = JsonUtils::to_json(dict);
}

void HsmObjectJsonAdapter::from_string(
    const std::string& input, HsmObject& item) const
{
    auto dict = JsonUtils::from_json(input);
    from_dict(*dict, item);
}

void HsmObjectJsonAdapter::from_string(
    const std::string& json, std::vector<HsmObject>& items) const
{
    auto dict = JsonUtils::from_json(json);
    if (dict->get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& entry : dict->get_sequence()) {
            items.emplace_back(HsmObject());
            from_dict(*entry, items[items.size() - 1]);
        }
    }
}

}  // namespace hestia