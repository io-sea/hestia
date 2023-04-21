#include "HsmAction.h"

#include <ostk/JsonUtils.h>

HsmAction::HsmAction() {}

void HsmAction::set_action(const std::string& action)
{
    if (action == "release") {
        m_action = Action::RELEASE;
    }
    else if (action == "copy") {
        m_action = Action::COPY;
    }
    else if (action == "move") {
        m_action = Action::MOVE;
    }
}