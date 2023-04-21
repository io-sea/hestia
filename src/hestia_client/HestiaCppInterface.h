#pragma once

#include <ostk/Extent.h>
#include <ostk/ReadableBufferView.h>
#include <ostk/Uuid.h>
#include <ostk/WriteableBufferView.h>

namespace hestia {

class HestiaCppInterface {
  public:
    static int copy(
        const ostk::Uuid& object_id,
        const ostk::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);

    static int get(
        const ostk::Uuid& object_id,
        const ostk::Extent& extent,
        ostk::WriteableBufferView& buffer,
        uint8_t tier);

    static int get_attributes(
        const ostk::Uuid& object_id,
        const std::string& keys,
        std::string& attributes);

    static int list_attributes(
        const ostk::Uuid& object_id, std::string& attributes);

    static int list_objects(uint8_t tier, std::vector<ostk::Uuid> objects);

    static int list_tiers(
        const ostk::Uuid& object_id, std::vector<uint8_t>& tiers);

    static int move(
        const ostk::Uuid& object_id,
        const ostk::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);

    static int put(
        const ostk::Uuid& object_id,
        const ostk::Extent& extent,
        const ostk::ReadableBufferView& buffer,
        uint8_t tier_id,
        bool overwrite);

    static int release(const ostk::Uuid& object_id);

    static int release(
        const ostk::Uuid& object_id, uint8_t tier_id, bool remove_key = true);

    static int set_attributes(
        const ostk::Uuid& object_id, const std::string& attributes);
};
}  // namespace hestia