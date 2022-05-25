#pragma once

#include "types.h"
#include <list>

namespace hestia {

/* TODO: tiers should not be defined here */
std::list<struct hsm_tier> tiers{
    hsm_tier(0, ssd_nvme, 1000, 5000), hsm_tier(1, tape, 10000, 500)};

}  // namespace hestia
