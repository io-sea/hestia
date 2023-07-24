#include "HsmEvent.h"

namespace hestia {
HsmEvent::HsmEvent() :
    HsmItem(HsmItem::Type::EVENT), Model(HsmItem::hsm_event_name)
{
}

std::string HsmEvent::get_type()
{
    return HsmItem::hsm_event_name;
}
}  // namespace hestia