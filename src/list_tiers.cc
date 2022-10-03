#include "../test/data_placement_engine/eejit.h"
#include "hestia.h"
#include "tiers.h"
#include <iostream>

std::vector<uint8_t> hestia::list_tiers()
{
    dpe::Eejit dpe(tiers);

    return dpe.list_tiers();
}
