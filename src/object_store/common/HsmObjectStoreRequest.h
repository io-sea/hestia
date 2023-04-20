#pragma once

#include <ostk/Uuid.h>
#include <ostk/ObjectStoreRequest.h>

enum class HsmObjectStoreRequestMethod
{
    PUT,
    GET,
    REMOVE,
    REMOVE_ALL,
    COPY,
    MOVE
};

class HsmObjectStoreRequest : public ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>
{
public:
    HsmObjectStoreRequest(const std::string& objectId, HsmObjectStoreRequestMethod method);
    HsmObjectStoreRequest(const ostk::StorageObject& object, HsmObjectStoreRequestMethod method);
    HsmObjectStoreRequest(const ostk::Uuid& objectId, HsmObjectStoreRequestMethod method);
    HsmObjectStoreRequest(const ostk::ObjectStoreRequest& request);

    bool isHsmOnlyRequest() const;

    std::string methodAsString() const override;

    void setSourceTier(uint8_t tier)
    {
        mSourceTier = tier;
    }

    void setTargetTier(uint8_t tier)
    {
        mTargetTier = tier;
    }

    uint8_t sourceTier() const;
    uint8_t targetTier() const;

    std::string toString() const;

    static ostk::ObjectStoreRequest toBaseRequest(const HsmObjectStoreRequest& reqeust);
    static bool isHsmSupportedMethod(ostk::ObjectStoreRequestMethod method);
    static std::string toString(HsmObjectStoreRequestMethod method);
private:
    static bool isCopyOrMoveRequest(HsmObjectStoreRequestMethod method);

    static HsmObjectStoreRequestMethod fromBaseMethod(ostk::ObjectStoreRequestMethod method);
    static ostk::ObjectStoreRequestMethod toBaseMethod(HsmObjectStoreRequestMethod method);

    uint8_t mTargetTier{0};
    uint8_t mSourceTier{0};
};