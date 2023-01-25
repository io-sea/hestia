#include <catch2/catch.hpp>
#include <hestia.h>
#include <string>

#include "common.h"

SCENARIO("Release interfaces correctly with the object store backend"
         "[object][store]")
{
    GIVEN("an object within the object store")
    {
        auto data_vec = GENERATE(
            chunk(max_data_size, take(max_data_size, random(' ', 'z'))));

        std::string data(data_vec.begin(), data_vec.end());

        auto hsm_uint_parts = GENERATE(chunk(
            2, take(
                   2, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));
        struct hestia::hsm_uint oid(hsm_uint_parts[0], hsm_uint_parts[1]);

        const int dest_tier = 1;
        hestia::put(oid, false, data.data(), 0, data.size(), 1);
        /*
                const auto json_attrs =
                    nlohmann::json::parse(hestia::get_attrs(oid, "tier"));
                const int dest_tier = json_attrs["tier"];
        */
        WHEN("The data is released from the current tier")
        {
            hestia::set_attrs(
                oid,
                R"({"trigger_migration": {"operation":"release", "src_tier":1}})");


            THEN(
                "The data associated with that object is deleted from the corresponding tier of the object store")
            {

                std::string copy_data;
                copy_data.resize(data.size());

                REQUIRE(
                    hestia::get(
                        oid, &copy_data[0], 0, copy_data.size(), 0, dest_tier)
                    == 2);
                // The error code 2 for get is thrown when the object is found
                // in the kvs but its data is not found on the specified tier of
                // the object store
            }
        }
        hestia::remove(oid);
    }
}
