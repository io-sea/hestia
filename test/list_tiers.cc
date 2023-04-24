#include "common.h"
#include "data_placement_engine/eejit.h"
#include "tiers.h"
#include <catch2/catch_all.hpp>
#include <hestia.h>
#include <list>

SCENARIO(
    "List_tiers correctly displays info about tiers from the data placement engine")
{
    GIVEN("a data placement engine with its tiers")
    {
        auto all_info = hestia::tiers;
        WHEN("List tiers is called")
        {
            auto retrieved_info = hestia::list_tiers();
            THEN(
                "The data retrieved from list_tiers contains the same list of tier ids as the data retrieved from the dpe directly")
            {
                REQUIRE(retrieved_info.size() == all_info.size());
                for (const auto& it : all_info) {
                    REQUIRE(
                        std::find(
                            retrieved_info.begin(), retrieved_info.end(), it.id)
                        != retrieved_info.end());
                }
            }
        }
    }
}
