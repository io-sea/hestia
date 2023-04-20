#pragma once

#include <ostk/Extent.h>
#include <ostk/Uuid.h>
#include <ostk/ReadableBufferView.h>
#include <ostk/WriteableBufferView.h>

namespace hestia{

class HestiaCppInterface
{
public:
    static int copy(const ostk::Uuid& objectId, const ostk::Extent& extent, uint8_t sourceTier, uint8_t targetTier);

    static int get(const ostk::Uuid& objectId, const ostk::Extent& extent, ostk::WriteableBufferView& buffer, uint8_t tier);

    static int getAttributes(const ostk::Uuid& objectId, const std::string& keys, std::string& attributes);

    static int listAttributes(const ostk::Uuid& objectId, std::string& attributes);

    static int listObjects(uint8_t tier, std::vector<ostk::Uuid> objects);

    static int listTiers(const ostk::Uuid& objectId, std::vector<uint8_t>& tiers);

    static int move(const ostk::Uuid& objectId, const ostk::Extent& extent, uint8_t sourceTier, uint8_t targetTier);

    static int put(const ostk::Uuid& objectId, const ostk::Extent& extent, const ostk::ReadableBufferView& buffer, 
        uint8_t tierId, bool overwrite);

    static int release(const ostk::Uuid& objectId);  

    static int release(const ostk::Uuid& objectId, uint8_t tierId, bool removeKey = true);        

    static int setAttributes(const ostk::Uuid& objectId, const std::string& attributes);
};
}