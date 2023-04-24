#include "HsmActionAdapter.h"

#include <ostk/JsonUtils.h>

HsmActionAdapter::Ptr HsmActionAdapter::create()
{
    return std::unique_ptr<HsmActionAdapter>();
}

void HsmActionAdapter::parse(const std::string& input, HsmAction& action)
{
    ostk::Metadata metadata;
    ostk::JsonUtils::from_json(input, metadata);

    if (const auto migration =
            metadata.get_item(HsmAction::trigger_migration_key);
        !migration.empty()) {
        auto parsed = ostk::JsonUtils::get_values(
            migration, {"operation", "src_tier", "tgt_tier"});

        if (parsed[0].empty() || parsed[1].empty()) {
            throw std::runtime_error("Failed to parse actions json");
        }

        action.set_action(parsed[0]);
        action.m_source_tier = std::stoi(parsed[1]);

        if (action.m_action != HsmAction::Action::RELEASE
            && parsed[2].empty()) {
            throw std::runtime_error("Failed to parse actions json");
        }
        action.m_target_tier = std::stoi(parsed[2]);
    }
}