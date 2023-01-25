#include <catch2/catch.hpp>
#include <hestia.h>
#include <string>

#include "common.h"
#include "tiers.h"

SCENARIO(
    "Put and Get interface correctly with the object store backend",
    "[object][store]")
{
    GIVEN("a string representing data to be stored")
    {
        auto data_vec = GENERATE(
            chunk(max_data_size, take(max_data_size, random(' ', 'z'))));

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
        WHEN("offset data is sent to the object store")
        {
            auto hsm_uint_parts = GENERATE(chunk(
                2, take(
                       2, random(
                              std::numeric_limits<std::uint64_t>::min(),
                              std::numeric_limits<std::uint64_t>::max()))));
            struct hestia::hsm_uint oid(hsm_uint_parts[0], hsm_uint_parts[1]);

            const std::size_t offset = GENERATE_COPY(take(
                1, random(
                       std::numeric_limits<std::size_t>::min() + 1,
                       data.size() - 1)));
            const std::size_t length = GENERATE_COPY(take(
                1, random(
                       std::numeric_limits<std::size_t>::min() + 1,
                       data.size() - offset)));

            const int dest_tier = 0;
            hestia::put(oid, false, data.data(), offset, length, dest_tier);
            /*
                        const auto json_attrs =
                            nlohmann::json::parse(hestia::get_attrs(oid,
              "tier")); int dest_tier = -1;

                        for(int i=0; i<static_cast<int>(hestia::tiers.size());
              ++i){ if(json_attrs["tiers"][i] == true){ dest_tier=i;
                                }
                        }
              //          REQUIRE(!(dest_tier < 0));
            */
            /*
            THEN("a hsm_obj is created")
            {
                REQUIRE(obj.oid == oid);
                REQUIRE(!obj.meta_data.empty());
            }
            */
            WHEN("the data is retrieved")
            {
                std::string recv_data;
                recv_data.resize(data.size());
                hestia::get(
                    oid, /* &obj,*/ &recv_data[0], offset, length, 0,
                    dest_tier);

                THEN("the stored data matches the retrieved data")
                {
                    REQUIRE(
                        recv_data.substr(offset, length)
                        == data.substr(offset, length));
                }
            }
            hestia::remove(oid);
        }
    }
}
