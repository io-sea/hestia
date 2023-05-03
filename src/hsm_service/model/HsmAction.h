#pragma once

#include <string>

namespace hestia {
class HsmAction {
  public:
    enum class Action { MOVE, COPY, RELEASE, NONE };

    HsmAction();

    bool has_action() const { return m_action != Action::NONE; }

    void set_action(const std::string& action);

    Action m_action{Action::NONE};
    std::uint8_t m_source_tier{0};
    std::uint8_t m_target_tier{0};

    static constexpr char trigger_migration_key[] = "trigger_migration";
};
}  // namespace hestia