#include "HsmActionAdapter.h"

#include <ostk/JsonUtils.h>

HsmActionAdapter::Ptr HsmActionAdapter::Create()
{
    return std::unique_ptr<HsmActionAdapter>();
}

void HsmActionAdapter::parse(const std::string& input, HsmAction& action)
{
    ostk::Metadata metadata;
    ostk::JsonUtils::fromJson(input, metadata);

    if (const auto migration = metadata.getItem(HsmAction::TRIGGER_MIGRATION_KEY); !migration.empty())
    {
        auto parsed = ostk::JsonUtils::getValues(migration, {"operation", "src_tier", "tgt_tier"});

        if (parsed[0].empty() || parsed[1].empty() )
        {
            throw std::runtime_error("Failed to parse actions json");
        }

        action.setAction(parsed[0]);
        action.mSourceTier = std::stoi(parsed[1]);

        if (action.mAction != HsmAction::Action::RELEASE && parsed[2].empty())
        {
            throw std::runtime_error("Failed to parse actions json");
        }
        action.mTargetTier = std::stoi(parsed[2]);
    }
}