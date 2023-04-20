#pragma once

#include <string>

class HsmAction
{
public:
    enum class Action
    {
        MOVE,
        COPY,
        RELEASE,
        NONE
    };

    HsmAction();

    bool hasAction() const
    {
        return mAction != Action::NONE;
    }

    void setAction(const std::string& action);

    Action mAction{Action::NONE};
    std::uint8_t mSourceTier{0};
    std::uint8_t mTargetTier{0};

    static constexpr char TRIGGER_MIGRATION_KEY[] = "trigger_migration";
};