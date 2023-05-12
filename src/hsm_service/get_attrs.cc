#include "hestia.h"
#include "hsm_key_value_store/disk.h"
#include <string>

std::string hestia::get_attrs(const struct hsm_uint& oid, const char* attr_keys)
{
    kv::Disk kv_store;

    if (!kv_store.object_exists(oid)) {
        return "";
    }

    return kv_store.get_meta_data(oid, attr_keys).dump();
}

std::vector<hestia::hsm_uint> hestia::get_dataset_members(
    const struct hsm_uint& oid)
{
    kv::Disk kv_store;
    std::vector<hsm_uint> members{};
    nlohmann::json dataset_attr = kv_store.get_meta_data(oid, "dataset");
    if (dataset_attr["dataset"] == false) {
        return members;
    }
    nlohmann::json members_attr = kv_store.get_meta_data(oid, "members");

    for (unsigned i = 0; i < members_attr["members"].size(); i++) {
        std::size_t pos  = 0;
        std::string mems = members_attr["members"][i];
        auto next_pos    = mems.find('-', pos);
        hsm_uint oid;
        auto upper = mems.substr(pos, next_pos - pos);
        pos        = next_pos + 1;
        auto lower = mems.substr(pos);
        oid        = hsm_uint(stoull(upper), stoull(lower));
        members.push_back(oid);
    }

    return members;
}
