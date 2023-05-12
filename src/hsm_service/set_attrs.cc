#include "hestia.h"
#include "hsm_key_value_store/disk.h"
#include <chrono>
#include <iostream>
int hestia::set_attrs(const struct hsm_uint& oid, const char* attrs)
{
    auto attrs_json = nlohmann::json::parse(attrs);
    // tgt_tier and operation must be specified, src_tier can be found using
    // get_attr
    if (attrs_json.contains("trigger_migration")) {
        if (!attrs_json["trigger_migration"].contains("operation")) {
            std::cout << "Error, operation must be specified\n";
            return 1;
        }
        if (!attrs_json["trigger_migration"].contains("src_tier")) {
            std::cout << "Error, source tier must be specified\n";
            return 1;
        }
        if (attrs_json["trigger_migration"]["operation"] == "release") {
            release(oid, attrs_json["trigger_migration"]["src_tier"]);
        }
        if (!attrs_json["trigger_migration"].contains("tgt_tier")) {
            std::cout << "Error, target tier must be specified\n";
            return 1;
        }
        if (attrs_json["trigger_migration"]["operation"] == "move") {
            move(
                oid, attrs_json["trigger_migration"]["src_tier"],
                attrs_json["trigger_migration"]["tgt_tier"]);
        }
        if (attrs_json["trigger_migration"]["operation"] == "copy") {
            copy(
                oid, attrs_json["trigger_migration"]["src_tier"],
                attrs_json["trigger_migration"]["tgt_tier"]);
        }
        // At this point, we need to check if the DPE will complete the move

        attrs_json.erase("trigger_migration");
        // Do the metadata update as part of copy, then add functionality to
        // delete the trigger migration attribute here
    }
    /* interface with kv store */
    kv::Disk kv_store;

    if (!kv_store.object_exists(oid)) {
        return 1;
    }

    kv_store.put_meta_data(oid, attrs_json);

    return 0;
}
