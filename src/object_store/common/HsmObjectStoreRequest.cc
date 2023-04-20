#include "HsmObjectStoreRequest.h"

#include <sstream>

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const std::string& objectId, HsmObjectStoreRequestMethod method) :
    ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(
        objectId, method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const ostk::StorageObject& object, HsmObjectStoreRequestMethod method) :
    ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(object, method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const ostk::Uuid& objectId, HsmObjectStoreRequestMethod method) :
    ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(
        objectId.toString(), method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const ostk::ObjectStoreRequest& request) :
    ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(
        request, fromBaseMethod(request.method()))
{
}

ostk::ObjectStoreRequest HsmObjectStoreRequest::toBaseRequest(
    const HsmObjectStoreRequest& reqeust)
{
    if (reqeust.isHsmOnlyRequest()) {
        throw std::runtime_error(
            "Attempted to convert a HSM operation to base type.");
    }

    ostk::ObjectStoreRequest base_request(
        reqeust.object(), toBaseMethod(reqeust.method()));
    base_request.setExtent(reqeust.extent());
    return base_request;
}

HsmObjectStoreRequestMethod HsmObjectStoreRequest::fromBaseMethod(
    ostk::ObjectStoreRequestMethod method)
{
    switch (method) {
        case ostk::ObjectStoreRequestMethod::GET:
            return HsmObjectStoreRequestMethod::GET;
        case ostk::ObjectStoreRequestMethod::PUT:
            return HsmObjectStoreRequestMethod::PUT;
        case ostk::ObjectStoreRequestMethod::REMOVE:
            return HsmObjectStoreRequestMethod::REMOVE;
        default:
            throw std::runtime_error(
                "Given unsupported base request type for hsm request type conversion.");
    }
}

ostk::ObjectStoreRequestMethod HsmObjectStoreRequest::toBaseMethod(
    HsmObjectStoreRequestMethod method)
{
    switch (method) {
        case HsmObjectStoreRequestMethod::GET:
            return ostk::ObjectStoreRequestMethod::GET;
        case HsmObjectStoreRequestMethod::PUT:
            return ostk::ObjectStoreRequestMethod::PUT;
        case HsmObjectStoreRequestMethod::REMOVE:
            return ostk::ObjectStoreRequestMethod::REMOVE;
        default:
            return ostk::ObjectStoreRequestMethod::CUSTOM;
    }
}

bool HsmObjectStoreRequest::isCopyOrMoveRequest(
    HsmObjectStoreRequestMethod method)
{
    return method == HsmObjectStoreRequestMethod::MOVE
           || method == HsmObjectStoreRequestMethod::COPY;
}

bool HsmObjectStoreRequest::isHsmSupportedMethod(
    ostk::ObjectStoreRequestMethod opType)
{
    return opType == ostk::ObjectStoreRequestMethod::GET
           || opType == ostk::ObjectStoreRequestMethod::PUT
           || opType == ostk::ObjectStoreRequestMethod::REMOVE;
}

bool HsmObjectStoreRequest::isHsmOnlyRequest() const
{
    return isCopyOrMoveRequest(mMethod);
}

std::string HsmObjectStoreRequest::methodAsString() const
{
    return toString(mMethod);
}

std::string HsmObjectStoreRequest::toString() const
{
    std::stringstream sstr;
    sstr << "ObjectId: " << object().id() << " | ";
    sstr << "Operation: " << toString(mMethod) << " | ";
    sstr << "Extent: " << extent().toString() << " | ";
    sstr << "Source Tier: " << mSourceTier << " | ";
    sstr << "mTargetTier Tier: " << mTargetTier;
    return sstr.str();
}

std::string HsmObjectStoreRequest::toString(HsmObjectStoreRequestMethod optType)
{
    switch (optType) {
        case HsmObjectStoreRequestMethod::PUT:
            return "PUT";
        case HsmObjectStoreRequestMethod::GET:
            return "GET";
        case HsmObjectStoreRequestMethod::REMOVE:
            return "REMOVE";
        case HsmObjectStoreRequestMethod::COPY:
            return "COPY";
        case HsmObjectStoreRequestMethod::MOVE:
            return "MOVE";
        default:
            return "UNKNOWN";
    }
}

uint8_t HsmObjectStoreRequest::sourceTier() const
{
    return mSourceTier;
}

uint8_t HsmObjectStoreRequest::targetTier() const
{
    return mTargetTier;
}