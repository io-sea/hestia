#include "./kv_store/disk.h"
#include "common.h"
#include <catch2/catch.hpp>
#include <cwctype>
#include <hestia.h>
#include <string>

SCENARIO(
    "Put_attrs and get_attrs interface correctly with the key-value store backend [key][value][store]")
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

        std::string attr_keys;
        nlohmann::json kvs;
        auto key_it   = key_vecs.begin();
        auto value_it = value_vecs.begin();
        while (key_it != key_vecs.end()) {
            kvs[*key_it] = *value_it;
            attr_keys.append(*key_it);
            attr_keys.push_back(delim);
            ++key_it;
            ++value_it;
        }
        attr_keys.pop_back();
        // remove last semicolon, otherwise it looks like there is another key
        auto attrs = kvs.dump();

        WHEN("the list of attributes is sent to the key-value store")
        {
            hestia::set_attrs(oid, attrs.data());

            WHEN(
                "The list of attributed is retrieved back from the key-value store")
            {
                {
                    auto recv_data = hestia::get_attrs(oid, attr_keys.data());
                    auto recv_data_json = nlohmann::json::parse(recv_data);
                    THEN(
                        "The retrieved list of attributes matches the list of attributes originally submitted")
                    {
                        auto delta = nlohmann::json::diff(kvs, recv_data_json);
                        REQUIRE(delta.empty());
                    }
                }
            }
        }
        hestia::remove(oid);
    }
}
