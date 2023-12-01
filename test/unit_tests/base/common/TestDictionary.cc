#include <catch2/catch_all.hpp>

#include "DictField.h"
#include "Dictionary.h"

TEST_CASE("Test Dictionary - Nested queries", "[dictionary]")
{
    hestia::Dictionary dict;

    std::unordered_map<std::string, std::string> data{{"mykey", "myval"}};

    dict.set_map(data);

    hestia::KeyValuePair query{"mykey", "myval"};

    REQUIRE(dict.has_key_and_value(query));
}

TEST_CASE("Test Dictionary - Merge", "[dictionary]")
{
    hestia::Dictionary dict0;
    std::unordered_map<std::string, std::string> data0{{"mykey0", "myval0"}};
    dict0.set_map(data0);

    hestia::Dictionary dict1;
    std::unordered_map<std::string, std::string> data1{{"mykey1", "myval1"}};
    dict1.set_map(data1);

    dict0.merge(dict1);

    REQUIRE(dict0.get_map().size() == 2);
    REQUIRE(dict0.get_map_item("mykey1")->get_scalar() == "myval1");
}

TEST_CASE("Test Dictionary - Flatten and Expand", "[dictionary]")
{
    auto dict0 = std::make_unique<hestia::Dictionary>();
    std::unordered_map<std::string, std::string> data0{
        {"mykey0", "myval0"}, {"mykey1", "myval1"}};
    dict0->set_map(data0);

    auto dict1 = std::make_unique<hestia::Dictionary>();
    std::unordered_map<std::string, std::string> data1{
        {"mykey2", "myval2"}, {"mykey3", "myval3"}};
    dict1->set_map(data1);

    auto dict2 = std::make_unique<hestia::Dictionary>();
    std::unordered_map<std::string, std::string> data2{
        {"mykey4", "myval4"}, {"mykey5", "myval5"}};
    dict2->set_map(data2);

    auto seq = std::make_unique<hestia::Dictionary>(
        hestia::Dictionary::Type::SEQUENCE);
    seq->add_sequence_item(std::move(dict0));
    seq->add_sequence_item(std::move(dict1));
    seq->add_sequence_item(std::move(dict2));

    auto outer_map = std::make_unique<hestia::Dictionary>();
    outer_map->set_map_item("mymap", std::move(seq));

    hestia::Map flattened;
    outer_map->flatten(flattened);

    hestia::Dictionary expanded;
    expanded.expand(flattened);

    REQUIRE(expanded == *outer_map);
}

TEST_CASE("Test Dictionary - From String", "[dictionary]")
{
    std::string raw =
        "creation_time = 1697562922730679, dataset.id = c03b78d6-5097-5074-6fe2-7e9aea24b5ed, tiers.seq_0.write_lock.max_lock_time = 5000, tiers.seq_0.tier.id = fcdec7bf-e4a9-16b1-371d-d71724169201, tiers.seq_0.read_lock.active = false, tiers.seq_0.write_lock.locked_at = 0, tiers.seq_0.backend.id = 0, tiers.seq_0.object.id = 00000000-0000-0000-0000-0000000004d2, tiers.seq_0.extents.seq_0.offset = 0, tiers.seq_0.extents.seq_0.length = 43, id = 00000000-0000-0000-0000-0000000004d2, tiers.seq_0.creation_time = 1697562922746573, tiers.seq_0.tier_name = 0, read_lock.active = false, type = object, tiers.seq_0.read_lock.locked_at = 0, read_lock.locked_at = 0, tiers.seq_0.write_lock.active = false, user_metadata.id = 2817a6e9-237b-e674-00be-951d7f650f2c, size = 43, read_lock.max_lock_time = 5000, tiers.seq_0.id = 1e76594a-f125-697f-2805-775b40b8f83a, tiers.seq_0.read_lock.max_lock_time = 5000, last_modified_time = 1697562922755912, write_lock.locked_at = 0, user_metadata.last_modified_time = 1697562922730653, user_metadata.type = metadata, user_metadata.object.id = 00000000-0000-0000-0000-0000000004d2, tiers.seq_0.last_modified_time = 1697562922746573, tiers.seq_0.type = extent, write_lock.active = false, user_metadata.creation_time = 1697562922730653, write_lock.max_lock_time = 5000,";

    hestia::Map::FormatSpec format;
    format.m_kv_delimiter   = '=';
    format.m_pair_delimiter = ',';

    hestia::Map flat(raw, format);

    hestia::Dictionary::FormatSpec dict_format;

    hestia::Dictionary dict(flat, dict_format);

    REQUIRE(dict.get_map_item("tiers")->get_sequence().size() == 1);
}

TEST_CASE("Test Dictionary - Failing", "[dictionary]")
{
    std::string input = R"(creation_time = 1698232585860259"
dataset.id = 2d6da775-aec1-4cd3-1a5a-25e63498f323
tiers.seq_3.write_lock.active = false
tiers.seq_3.tier.id = ca04b33d-6da6-e10b-ab85-1d6c17ce71d0
tiers.seq_3.tier_name = 0
tiers.seq_3.type = extent
tiers.seq_3.read_lock.active = false
tiers.seq_3.object.id = 00000000-0000-0000-0000-0000000004d2
tiers.seq_3.last_modified_time = 1698232585865253
tiers.seq_3.extents.seq_0.length = 0
tiers.seq_3.backend.id = 0
tiers.seq_2.write_lock.active = false
tiers.seq_2.read_lock.max_lock_time = 5000
tiers.seq_2.read_lock.locked_at = 0
tiers.seq_2.read_lock.active = false
tiers.seq_2.object.id = 00000000-0000-0000-0000-0000000004d2
tiers.seq_2.last_modified_time = 1698232585861460
id = 00000000-0000-0000-0000-0000000004d2
tiers.seq_3.read_lock.max_lock_time = 5000
tiers.seq_2.id = 7db3c3c8-32e1-2fec-e49a-508800b29fd3
tiers.seq_2.creation_time = 1698232585861460
tiers.seq_3.write_lock.locked_at = 0
tiers.seq_2.write_lock.locked_at = 0
tiers.seq_2.backend.id = 0
tiers.seq_2.tier.id = 76114b47-d4f7-271f-c396-ce24c25cd0d0
tiers.seq_1.write_lock.max_lock_time = 5000
tiers.seq_1.tier.id = 5aa26651-655e-db67-7283-51ad84ba1ed2
tiers.seq_1.read_lock.max_lock_time = 5000
tiers.seq_1.last_modified_time = 1698232585864979
tiers.seq_1.object.id = 00000000-0000-0000-0000-0000000004d2
tiers.seq_0.write_lock.max_lock_time = 5000
tiers.seq_1.id = 6e177b8b-0431-0fe5-d7a2-031a92d1cd40
tiers.seq_1.extents.seq_0.offset = 0
tiers.seq_3.creation_time = 1698232585863863
tiers.seq_0.creation_time = 1698232585862869
tiers.seq_1.creation_time = 1698232585864979
tiers.seq_1.backend.id = 0
tiers.seq_0.read_lock.locked_at = 0
tiers.seq_0.tier.id = 258f681f-3ab1-3653-8820-5820010e76a9
tiers.seq_3.write_lock.max_lock_time = 5000
read_lock.max_lock_time = 5000
tiers.seq_0.read_lock.active = false
last_modified_time = 1698232585865547
tiers.seq_0.write_lock.locked_at = 0
tiers.seq_3.id = f5324b2f-a0f6-5d97-abe1-78de601122d6
tiers.seq_0.backend.id = 0
tiers.seq_0.object.id = 00000000-0000-0000-0000-0000000004d2
write_lock.max_lock_time = 5000
tiers.seq_0.extents.seq_0.offset = 0
tiers.seq_2.type = extent
tiers.seq_0.extents.seq_0.length = 43
write_lock.locked_at = 0
tiers.seq_0.tier_name = 0
tiers.seq_2.extents.seq_0.length = 43
read_lock.active = false
tiers.seq_1.type = extent
tiers.seq_2.tier_name = 0
type = object
tiers.seq_2.write_lock.max_lock_time = 5000
user_metadata.type = metadata
user_metadata.last_modified_time = 1698232585860244
tiers.seq_1.write_lock.active = false
read_lock.locked_at = 0
tiers.seq_3.extents.seq_0.offset = 0
tiers.seq_0.write_lock.active = false
user_metadata.id = b700f85c-09b7-2aba-c4d5-2c510c829308
size = 43
tiers.seq_1.tier_name = 0
tiers.seq_0.id = 3f6c70c4-c883-3242-7daa-1ffbca8bbb74
tiers.seq_1.read_lock.locked_at = 0
tiers.seq_0.read_lock.max_lock_time = 5000
tiers.seq_3.read_lock.locked_at = 0
tiers.seq_2.extents.seq_0.offset = 0
tiers.seq_1.read_lock.active = false
user_metadata.object.id = 00000000-0000-0000-0000-0000000004d2
tiers.seq_0.last_modified_time = 1698232585862869
tiers.seq_1.write_lock.locked_at = 0
tiers.seq_0.type = extent
write_lock.active = false
tiers.seq_1.extents.seq_0.length = 43
user_metadata.creation_time = 1698232585860244)";

    hestia::Dictionary::FormatSpec format;
    format.m_map_format.m_kv_delimiter   = '=';
    format.m_map_format.m_pair_delimiter = '\n';

    hestia::Dictionary dict(input, format);

    REQUIRE(dict.get_map_item("tiers")->get_sequence().size() == 4);
    for (const auto& tier : dict.get_map_item("tiers")->get_sequence()) {
        REQUIRE(tier->has_map_item("creation_time"));
    }
}

TEST_CASE("RawDictField- modify value and primary key", "[common]")
{
    hestia::RawDictField testfield{"Test Field"};

    hestia::Dictionary dict;

    std::unordered_map<std::string, std::string> data{
        {"mykey0", "myval0"}, {"mykey1", "myval1"}};

    dict.set_map(data);

    testfield.get_value_as_writeable() = dict;

    REQUIRE(testfield.modified() == true);

    REQUIRE(testfield.value().get_map().size() == 2);
    REQUIRE(testfield.value().get_map_item("mykey0")->get_scalar() == "myval0");
    REQUIRE(testfield.value().get_map_item("mykey1")->get_scalar() == "myval1");

    testfield.set_is_primary_key(true);
    REQUIRE(testfield.is_primary_key());
}

TEST_CASE("Test deserialize with whitespace", "[common]")
{
    std::string payload = "\ndata.my_key=my_value\n";

    hestia::Dictionary::FormatSpec default_spec;
    hestia::Dictionary dict(payload, default_spec);
    REQUIRE(dict.has_map_item("data"));
}