#include "HsmService.h"

#include "DataPlacementEngine.h"
#include "HsmStoreInterface.h"
#include "KeyValueStore.h"
#include "MultiBackendHsmObjectStoreClient.h"

#include "HsmActionAdapter.h"
#include "HsmObjectAdapter.h"

#define ERROR_CHECK(response)                                                  \
    if (!response->ok()) {                                                     \
        return HsmServiceResponse::Create(req, std::move(response));           \
    }

#define ON_ERROR(code, message)                                                \
    auto response = HsmServiceResponse::Create(req);                           \
    response->onError({HsmServiceErrorCode::code, msg});                       \
    return response;

HsmService::HsmService(
    std::unique_ptr<KeyValueStore> kvStore,
    std::unique_ptr<MultiBackendHsmObjectStoreClient> objectStore,
    std::unique_ptr<DataPlacementEngine> placementEngine) :
    mStore(
        HsmStoreInterface::Create(std::move(kvStore), std::move(objectStore))),
    mDataPlacementEngine(std::move(placementEngine)),
    mObjectAdapter(HsmObjectAdapter::Create()),
    mActionAdapter(HsmActionAdapter::Create())
{
}

HsmService::~HsmService() {}

HsmServiceResponse::Ptr HsmService::makeRequest(
    const HsmServiceRequest& req, ostk::Stream* stream) noexcept
{
    switch (req.method()) {
        case HsmServiceRequestMethod::GET:
            return get(req, stream);
        case HsmServiceRequestMethod::PUT:
            return put(req, stream);
        case HsmServiceRequestMethod::COPY:
            return copy(req);
        case HsmServiceRequestMethod::MOVE:
            return move(req);
        case HsmServiceRequestMethod::REMOVE:
            return remove(req);
        case HsmServiceRequestMethod::REMOVE_ALL:
            return remove_all(req);
        default:
            return nullptr;
    }
}

HsmServiceResponse::Ptr HsmService::put(
    const HsmServiceRequest& req, ostk::Stream* stream) noexcept
{
    auto exists = mStore->exists(req.object());
    ERROR_CHECK(exists);
    if (exists->objectFound() != req.shouldOverwritePut()) {
        const std::string msg =
            "Overwrite request not compatible with object existence.";
        ON_ERROR(BAD_PUT_OVERWRITE_COMBINATION, msg);
    }

    if (stream) {
        const auto chosen_tier = mDataPlacementEngine->chooseTier(
            req.extent().mLength, req.targetTier());
        auto data_put_response = mStore->putData(
            req.object(), req.extent(), req.sourceTier(), chosen_tier,
            req.shouldOverwritePut(), stream);
        ERROR_CHECK(data_put_response);

        // Update metadata
        auto obj = req.object();
        HsmObject hsm_object(obj);
        hsm_object.addTier(chosen_tier);

        mObjectAdapter->sync(hsm_object, obj);
        auto metadata_put_response = mStore->putMetadata(obj);

        return HsmServiceResponse::Create(
            req, std::move(metadata_put_response));
    }

    if (!req.query().empty()) {
        HsmAction action;
        mActionAdapter->parse(req.query(), action);
        if (action.hasAction()) {
            // First finish this request and remove the metadata
            /*
            switch (action.mAction)
            {
                case HsmAction::Action::RELEASE:
                    HsmServiceRequest action_req(req.object(),
            HsmServiceRequestMethod::REMOVE);
                    action_req.setSourceTier(action.mSourceTier);
                    return makeRequest(action_req);
                case HsmAction::Action::COPY:
                    HsmServiceRequest action_req(req.object(),
            HsmServiceRequestMethod::COPY);
                    action_req.setSourceTier(action.mSourceTier);
                    action_req.setTargetTier(action.mTargetTier);
                    return makeRequest(action_req);
                    break;
                case HsmAction::Action::MOVE:
                    HsmServiceRequest action_req(req.object(),
            HsmServiceRequestMethod::MOVE);
                    action_req.setSourceTier(action.mSourceTier);
                    action_req.setTargetTier(action.mTargetTier);
                    return makeRequest(action_req);
                    break;
                    break;
                default:
                    break;
            }
            */
        }
    }

    auto metadata_put_response = mStore->putMetadata(req.object());
    return HsmServiceResponse::Create(req, std::move(metadata_put_response));
}

HsmServiceResponse::Ptr HsmService::get(
    const HsmServiceRequest& req, ostk::Stream* stream) noexcept
{
    auto exists = mStore->exists(req.object());
    ERROR_CHECK(exists);
    if (!exists->objectFound()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    if (stream) {
        auto response = mStore->getData(
            req.object(), req.extent(), req.sourceTier(), stream);
        return HsmServiceResponse::Create(req, std::move(response));
    }
    else {
        auto response = mStore->getMetadata(req.object());
        return HsmServiceResponse::Create(req, std::move(response));
    }
}

HsmServiceResponse::Ptr HsmService::copy(const HsmServiceRequest& req) noexcept
{
    auto exists = mStore->exists(req.object());
    ERROR_CHECK(exists);
    if (!exists->objectFound()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    auto obj = req.object();
    /*
    mKeyValueStore->fetch(obj, {"tiers"});
    */

    HsmObject hsm_object(obj);

    mObjectAdapter->parseTiers(hsm_object);

    auto copy_data_response = mStore->copyData(
        req.object(), req.extent(), req.sourceTier(), req.targetTier());
    ERROR_CHECK(copy_data_response);

    hsm_object.addTier(req.targetTier());
    mObjectAdapter->serialize(hsm_object);

    auto metadata_put_response = mStore->putMetadata(obj);
    return HsmServiceResponse::Create(req, std::move(metadata_put_response));
}

HsmServiceResponse::Ptr HsmService::move(const HsmServiceRequest& req) noexcept
{
    auto exists = mStore->exists(req.object());
    ERROR_CHECK(exists);
    if (!exists->objectFound()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    auto obj = req.object();
    /*
    mKeyValueStore->fetch(obj, {"tiers"});
    */

    HsmObject hsm_object(obj);

    mObjectAdapter->parseTiers(hsm_object);

    auto move_data_response = mStore->moveData(
        req.object(), req.extent(), req.sourceTier(), req.targetTier());
    ERROR_CHECK(move_data_response);

    hsm_object.addTier(req.targetTier());
    mObjectAdapter->serialize(hsm_object);

    auto metadata_put_response = mStore->putMetadata(obj);
    return HsmServiceResponse::Create(req, std::move(metadata_put_response));
}

HsmServiceResponse::Ptr HsmService::remove(
    const HsmServiceRequest& req) noexcept
{
    auto obj = req.object();

    /*
    mKeyValueStore->fetch(obj, {"tiers"});
    */

    HsmObject hsm_object(obj);
    mObjectAdapter->parseTiers(hsm_object);
    mObjectAdapter->serialize(hsm_object);

    auto remove_data_response =
        mStore->releaseData(req.object(), req.extent(), req.sourceTier());
    ERROR_CHECK(remove_data_response);

    hsm_object.removeTier(req.sourceTier());
    mObjectAdapter->serialize(hsm_object);

    auto metadata_put_response = mStore->putMetadata(obj);
    return HsmServiceResponse::Create(req, std::move(metadata_put_response));
}

HsmServiceResponse::Ptr HsmService::remove_all(
    const HsmServiceRequest& req) noexcept
{
    auto obj = req.object();

    /*
    mKeyValueStore->fetch(obj, {"tiers"});
    */

    HsmObject hsm_object(obj);
    mObjectAdapter->parseTiers(hsm_object);
    mObjectAdapter->serialize(hsm_object);

    auto remove_data_response = mStore->releaseData(req.object(), req.extent());
    ERROR_CHECK(remove_data_response);

    hsm_object.removeAllButOneTiers();
    mObjectAdapter->serialize(hsm_object);

    auto metadata_put_response = mStore->putMetadata(obj);
    return HsmServiceResponse::Create(req, std::move(metadata_put_response));
}

void HsmService::listObjects(uint8_t tier, std::vector<HsmObject>& objects)
{
    /*
    std::vector<ostk::StorageObject> objs;
    mKeyValueStore->list(tier, objs);
    /*/
}

void HsmService::listTiers(HsmObject& object, std::vector<uint8_t>& tiers)
{
    /*
    if (!mKeyValueStore->exists(object.mStorageObject))
    {
        throw std::runtime_error("Requested non-existing object");
    }

    mKeyValueStore->fetch(object.mStorageObject, {"tiers"});
    HsmObjectAdapter::parseTiers(object);
    tiers = object.mTierIds;
    */
}

void HsmService::listAttributes(HsmObject& object) {}
