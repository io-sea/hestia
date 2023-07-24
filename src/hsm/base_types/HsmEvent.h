#pragma once

#include "HsmItem.h"
#include "Model.h"

namespace hestia {
class HsmEvent : public HsmItem, public Model {
  public:
    HsmEvent();

    static std::string get_type();
};
}  // namespace hestia