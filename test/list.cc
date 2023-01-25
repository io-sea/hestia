#include "common.h"
#include "tiers.h"
#include <catch2/catch.hpp>
#include <hestia.h>
#include <string>
#include <vector>

SCENARIO("List functions correctly", "[kv][store]")
{
    GIVEN("a series of objects stored to the system")
    {
        std::vector<std::string> data_vecs(max_num_objects);
        auto data_pool = GENERATE(chunk(
            max_num_objects * max_data_size,
            take(max_num_objects * max_data_size, random(' ', 'z'))));

        auto data_it = data_pool.begin();
        for (auto& data_vec : data_vecs) {
            data_vec = std::string(data_it, std::next(data_it, max_data_size));
            std::advance(data_it, max_data_size);
        }

        auto hsm_uint_parts = GENERATE(chunk(
            2 * max_num_objects,
            take(
                2 * max_num_objects,
                random(
                    std::numeric_limits<std::uint64_t>::min(),
                    std::numeric_limits<std::uint64_t>::max()))));

        std::vector<struct hestia::hsm_uint> oids(max_num_objects);

        auto oid_it = hsm_uint_parts.begin() - 1;
        for (auto& oid : oids) {
            oid = hestia::hsm_uint(*(++oid_it), *(++oid_it));
        }

        /* store which tier each object is placed in */
        std::unordered_map<std::uint8_t, std::vector<struct hestia::hsm_uint>>
            oids_per_tier;
        for (unsigned int i = 0; i < max_num_objects; ++i) {
            const int dest_tier = 0;
            hestia::put(
                oids[i], false, data_vecs[i].data(), 0, data_vecs[i].size(), 0);
            //            const auto json_attrs =
            //              nlohmann::json::parse(hestia::get_attrs(oids[i],
            //              "tiers"));
            //            const int dest_tier = json_attrs["tier"];

            //            int dest_tier =
            //            static_cast<int>(hestia::locate(oids[i])[0]); for(int
            //            j=0; j< static_cast<int>((hestia::tiers).size());
            //            j++){
            //              if(json_attrs["tiers"][j] == true){
            //                dest_tier = j;
            //          }
            //    }
            oids_per_tier[dest_tier].push_back(oids[i]);
        }

        WHEN("a list of the IDs of the stored objects is retrieved")
        {
            /* use separate list for each tier */
            std::unordered_map<
                std::uint8_t, std::vector<struct hestia::hsm_uint>>
                recv_oids_per_tier;
            for (const auto& kv : oids_per_tier) {
                recv_oids_per_tier[kv.first] = hestia::list(kv.first);
            }

            THEN("the retrieved list matches to IDs of the stored objects")
            {
                /*
                 * there may be more objects than those added during this test
                 */
                for (const auto& kv : recv_oids_per_tier) {
                    REQUIRE(kv.second.size() >= oids_per_tier[kv.first].size());
                    for (const auto& oid : oids_per_tier[kv.first]) {
                        REQUIRE(
                            std::find(kv.second.begin(), kv.second.end(), oid)
                            != kv.second.end());
                    }
                }
            }
        }
    }
}
