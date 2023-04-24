#include "common.h"
#include "data_placement_engine/eejit.h"
#include "tiers.h"
#include <catch2/catch_all.hpp>
#include <hestia.h>
#include <list>

SCENARIO(
    "Get_tiers_info correctly displays info about tiers from the data placement engine")
{
    GIVEN("a data placement engine with its tiers")
    {
        auto all_info = hestia::tiers;
        WHEN("get_tiers_info is called for all tiers")
        {
            std::vector<int> tids;
            for (unsigned long i = 0; i < all_info.size(); i++) {
                tids.push_back(static_cast<int>(i));
            }

            auto retrieved_info = hestia::get_tiers_info(tids);
            THEN(
                "The data retrieved from get_tiers_info is the same as the data retrieved from the dpe directly")
            {
                REQUIRE(retrieved_info.size() == all_info.size());
                for (const auto& it : retrieved_info) {
                    REQUIRE(
                        std::find(all_info.begin(), all_info.end(), it)
                        != all_info.end());
                }
            }
        }
        WHEN("get_tiers_info is called for any individual tier")
        {
            THEN(
                "The data retrieved matches the data for the tier it was called for")
            {
                for (unsigned long i = 0; i < all_info.size(); i++) {
                    std::vector<int> tid{static_cast<int>(i)};
                    auto retrieved = hestia::get_tiers_info(tid);
                    REQUIRE(
                        std::find(
                            all_info.begin(), all_info.end(), retrieved.front())
                        != all_info.end());
                }
            }
        }
    }
}
