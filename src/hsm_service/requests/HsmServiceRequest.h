#pragma once

#include <ostk/Extent.h>
#include <ostk/StorageObject.h>
#include <ostk/Uuid.h>
#include <ostk/Request.h>

enum class HsmServiceRequestMethod
{
    PUT,
    GET,
    GET_TIERS,
    REMOVE,
    REMOVE_ALL,
    COPY,
    MOVE,
    LIST,
    LIST_TIERS
};

class HsmServiceRequest : public ostk::BaseRequest, public ostk::MethodRequest<HsmServiceRequestMethod>
{
public:
    HsmServiceRequest(const ostk::Uuid& objectId, HsmServiceRequestMethod method);

    HsmServiceRequest(const ostk::StorageObject& object, HsmServiceRequestMethod method);

    HsmServiceRequest(HsmServiceRequestMethod method);

    const ostk::Extent& extent() const
    {
        return mExtent;
    }

    const ostk::StorageObject& object() const;

    const std::string& query() const
    {
        return mQuery;
    }

    std::string methodAsString() const override
    {
        return {};
    }

    void setExtent(const ostk::Extent& extent);

    void setTargetTier(uint8_t tier);

    void setSourceTier(uint8_t tier);

    void setShouldPutOverwrite(bool overwrite);

    bool shouldOverwritePut() const;

    void setQuery(const std::string& query);

    uint8_t sourceTier() const
    {
        return mSourceTier;
    }

    uint8_t targetTier() const
    {
        return mTargetTier;
    }

private:
    uint8_t mTargetTier{0};
    uint8_t mSourceTier{0};
    ostk::Extent mExtent;
    ostk::StorageObject mObject;

    std::string mQuery;
    bool mOverwriteIfExisting{false};
};