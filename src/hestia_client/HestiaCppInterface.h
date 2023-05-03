#pragma once

#include "Extent.h"
#include "ReadableBufferView.h"
#include "Uuid.h"
#include "WriteableBufferView.h"

namespace hestia {

class HestiaCppInterface {
  public:
    static int copy(
        const hestia::Uuid& object_id,
        const hestia::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);

    static int get(
        const hestia::Uuid& object_id,
        const hestia::Extent& extent,
        hestia::WriteableBufferView& buffer,
        uint8_t tier);

    static int get_attributes(
        const hestia::Uuid& object_id,
        const std::string& keys,
        std::string& attributes);

    static int list_attributes(
        const hestia::Uuid& object_id, std::string& attributes);

    static int list_objects(uint8_t tier, std::vector<hestia::Uuid> objects);

    static int list_tiers(
        const hestia::Uuid& object_id, std::vector<uint8_t>& tiers);

    static int move(
        const hestia::Uuid& object_id,
        const hestia::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);

    static int put(
        const hestia::Uuid& object_id,
        const hestia::Extent& extent,
        const hestia::ReadableBufferView& buffer,
        uint8_t tier_id,
        bool overwrite);

    static int release(const hestia::Uuid& object_id);

    static int release(
        const hestia::Uuid& object_id, uint8_t tier_id, bool remove_key = true);

    static int set_attributes(
        const hestia::Uuid& object_id, const std::string& attributes);
};
}  // namespace hestia