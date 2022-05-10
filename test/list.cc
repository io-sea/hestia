#include <catch2/catch.hpp>
#include <hestia.h>
#include <string>

#include "common.h"

SCENARIO("List functions correctly", "[kv][store]")
{
    GIVEN("a series of objects stored to the system")
    {
        std::vector<std::string> data_vecs(max_num_objects);
        auto data_pool = GENERATE(chunk(
            max_num_objects * max_data_size,
            take(
                max_num_objects * max_data_size,
                random(
                    std::numeric_limits<char>::min(),
                    std::numeric_limits<char>::max()))));

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
        std::vector<struct hestia::hsm_obj> objs(max_num_objects);

        auto oid_it = hsm_uint_parts.begin() - 1;
        for (auto& oid : oids) {
            oid = hestia::hsm_uint(*(++oid_it), *(++oid_it));
        }

        for (unsigned int i = 0; i < max_num_objects; ++i) {
            hestia::put(
                oids[i], &objs[i], false, data_vecs[i].data(), 0,
                data_vecs[i].size(), 0);
        }

        WHEN("a list of the IDs of the stored objects is retrieved")
        {
            auto recv_oids = hestia::list();

            THEN("the retrieved list matches to IDs of the stored objects")
            {
                /*
                 * there may be more objects than those added during this test
                 */
                REQUIRE(recv_oids.size() >= oids.size());

                for (const auto& oid : oids) {
                    REQUIRE(
                        std::find(recv_oids.begin(), recv_oids.end(), oid)
                        != recv_oids.end());
                }
            }
        }
    }
}
