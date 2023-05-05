#include <catch2/catch_all.hpp>
#include <hestia.h>
#include <string>

#include "common.h"
#include "tiers.h"

SCENARIO(
    "Put and Get interface correctly with the object store backend for whole objects",
    "[object][store]")
{
    GIVEN("a string representing data to be stored")
    {
        auto data_vec = GENERATE(
            chunk(max_data_size, take(max_data_size, random(32, 122))));

        std::string data(data_vec.begin(), data_vec.end());

        WHEN("the entire data is sent to the object store")
        {
            auto hsm_uint_parts = GENERATE(chunk(
                2, take(
                       2, random(
                              std::numeric_limits<std::uint64_t>::min(),
                              std::numeric_limits<std::uint64_t>::max()))));
            struct hestia::hsm_uint oid(hsm_uint_parts[0], hsm_uint_parts[1]);

            const int dest_tier = 0;
            auto rc =
                hestia::put(oid, false, data.data(), 0, data.size(), dest_tier);

            REQUIRE(rc == 0);
            /*
                        const auto json_attrs =
                            nlohmann::json::parse(hestia::get_attrs(oid,
            "tiers")); int dest_tier = -1;

                        for(int i=0; i<static_cast<int>(hestia::tiers.size());
            ++i){ if(json_attrs["tiers"][i] == true){ dest_tier=i;
                                }
                        }
            //            REQUIRE(!(dest_tier < 0));
            */
            // THEN("a hsm_obj is created")
            //{
            //     REQUIRE(obj.oid == oid);
            //     REQUIRE(!obj.meta_data.empty());
            // }
            WHEN("the data is retrieved")
            {
                std::string recv_data;
                recv_data.resize(data.size());
                hestia::get(
                    oid, &recv_data[0], 0, recv_data.size(), 0, dest_tier);

                THEN("the stored data matches the retrieved data")
                {
                    REQUIRE(recv_data == data);
                }
            }
            hestia::remove(oid);
        }
    }
}

SCENARIO(
    "Put and Get interface correctly with the object store backend for parts of objects",
    "[object][store]")
{
    GIVEN("an object in the object store")
    {
        auto data_vec = GENERATE(
            chunk(max_data_size, take(max_data_size, random(32, 122))));

        std::string data(data_vec.begin(), data_vec.end());

        auto hsm_uint_parts = GENERATE(chunk(
            3, take(
                   3, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));
        struct hestia::hsm_uint oid(hsm_uint_parts[1], hsm_uint_parts[2]);

        const int dest_tier = 0;
        hestia::put(oid, false, data.data(), 0, data.size(), dest_tier);
        /*
                    const auto json_attrs =
                        nlohmann::json::parse(hestia::get_attrs(oid,
        "tiers")); int dest_tier = -1;

                    for(int i=0; i<static_cast<int>(hestia::tiers.size());
        ++i){ if(json_attrs["tiers"][i] == true){ dest_tier=i;
                            }
                    }
        //            REQUIRE(!(dest_tier < 0));
        */
        // THEN("a hsm_obj is created")
        //{
        //     REQUIRE(obj.oid == oid);
        //     REQUIRE(!obj.meta_data.empty());
        // }
        WHEN("Part of the object is overwritten")
        {
            auto offset = GENERATE(
                take(1, random(0, static_cast<int>(max_data_size) - 1)));
            auto end_offset    = GENERATE_REF(take(
                   1, filter(  // NOLINT
                       [&offset](auto var) { return var > offset; },
                       random(1, static_cast<int>(max_data_size)))));
            auto length        = end_offset - offset;
            auto overwrite_vec = GENERATE(
                chunk(max_data_size, take(max_data_size, random(32, 122))));

            std::string overwrite_data(
                overwrite_vec.begin(), overwrite_vec.begin() + length);

            hestia::put(
                oid, true, overwrite_data.data(), offset, length, dest_tier);
            WHEN("the overwritten part of the data is retrieved")
            {
                std::string recv_data;
                recv_data.resize(length);
                hestia::get(oid, &recv_data[0], offset, length, 0, dest_tier);

                THEN("the stored data matches the retrieved data")
                {
                    REQUIRE(recv_data == overwrite_data);
                }
            }
            hestia::remove(oid);
        }
    }
}
