#include "./kv_store/disk.h"
#include "common.h"
#include <catch2/catch.hpp>
#include <cwctype>
#include <hestia.h>
#include <string>

SCENARIO(
    "List_attrs correctly displays attributes retrieved from the key_value store")
{
    GIVEN(
        "a json string of keys and values to be stored and an object in the object store")
    {
        std::vector<std::string> key_vecs(max_num_objects);
        std::vector<std::string> value_vecs(max_num_objects);
        hestia::kv::Disk key_value_store;
        auto delim     = key_value_store.get_delim();
        auto data_pool = GENERATE_REF(chunk(
            2 * max_num_objects * max_data_size,
            take(
                2 * max_num_objects * max_data_size,
                filter(  // NOLINT
                    [&delim](auto var) { return var != delim; },
                    random(' ', 'z')))));

        // use the same data pool to fill both the vector of keys and the vector
        // of values
        auto data_it = data_pool.begin();
        for (auto& key_vec : key_vecs) {
            key_vec = std::string(data_it, std::next(data_it, max_data_size));
            std::advance(data_it, max_data_size);
        }
        for (auto& value_vec : value_vecs) {
            value_vec = std::string(data_it, std::next(data_it, max_data_size));
            std::advance(data_it, max_data_size);
        }

        auto hsm_uint_parts = GENERATE(chunk(
            2, take(
                   2, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));

        hestia::hsm_uint oid(hsm_uint_parts[0], hsm_uint_parts[1]);

        hestia::put(oid, false, "", 0, 0, 0);

        nlohmann::json kvs;
        auto key_it   = key_vecs.begin();
        auto value_it = value_vecs.begin();
        while (key_it != key_vecs.end()) {
            kvs[*key_it] = *value_it;
            ++key_it;
            ++value_it;
        }
        auto attrs = kvs.dump();

        WHEN("the list of attributes is sent to the key-value store")
        {
            hestia::set_attrs(oid, attrs.data());

            WHEN("The list of attributes is retrieved using list_attr")
            {
                {
                    auto recv_data      = hestia::list_attrs(oid);
                    auto recv_data_json = nlohmann::json::parse(recv_data);
                    auto patch          = nlohmann::json::parse(recv_data);
                    THEN(
                        "The list of attributes contains all those that were originally set by set_attrs")
                    {
                        patch.merge_patch(kvs);
                        // merge patch will add any keys not present in
                        // recv_data_json but present in kvs to recv_data_json
                        // and overwrite any keys that have a value different to
                        // that in kvs
                        auto delta =
                            nlohmann::json::diff(patch, recv_data_json);
                        // want the patch to be the same as the recieved data,
                        // which means that all the keys in kvs were present in
                        // the retrieved data and the values matched
                        REQUIRE(delta.empty());
                    }
                }
            }
        }
        hestia::remove(oid);
    }
}
