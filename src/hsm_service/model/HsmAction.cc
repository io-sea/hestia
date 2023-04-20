#include "HsmAction.h"

#include <ostk/JsonUtils.h>

HsmAction::HsmAction()
{

}

void HsmAction::setAction(const std::string& action)
{
    if (action == "release")
    {
        mAction = Action::RELEASE;
    }
    else if (action == "copy")
    {
        mAction = Action::COPY;
    }
    else if (action == "move")
    {
        mAction = Action::MOVE;
    }
}