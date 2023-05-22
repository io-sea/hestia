#include "HsmService.h"

#include "DataPlacementEngine.h"
#include "HsmKeyValueStore.h"
#include "HsmStoreInterface.h"
#include "MultiBackendHsmObjectStoreClient.h"

#include "HsmActionAdapter.h"
#include "HsmObjectAdapter.h"

#include "Logger.h"

#define ERROR_CHECK(response)                                                  \
    if (!response->ok()) {                                                     \
        return hestia::HsmServiceResponse::create(req, std::move(response));   \
    }

#define ON_ERROR(code, message)                                                \
    auto response = hestia::HsmServiceResponse::create(req);                   \
    response->on_error({hestia::HsmServiceErrorCode::code, msg});              \
    return response;

namespace hestia {
HsmService::HsmService(
    std::unique_ptr<HsmKeyValueStore> kv_store,
    std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store,
    std::unique_ptr<DataPlacementEngine> placement_engine,
    std::unique_ptr<HsmObjectAdapter> object_adatper) :
    m_store(HsmStoreInterface::create(
        std::move(kv_store), std::move(object_store))),
    m_data_placement_engine(std::move(placement_engine)),
    m_object_adapter(
        object_adatper ? std::move(object_adatper) :
                         HsmObjectAdapter::create()),
    m_action_adapter(HsmActionAdapter::create())
{
    LOG_INFO("Creating HsmService");
}
HsmService::Ptr HsmService::create(
    std::unique_ptr<HsmKeyValueStore> kv_store,
    std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store,
    std::unique_ptr<DataPlacementEngine> placement_engine)
{
    return std::make_unique<HsmService>(
        std::move(kv_store), std::move(object_store),
        std::move(placement_engine));
}


HsmService::~HsmService()
{
    LOG_INFO("Destroying HsmService");
}

HsmServiceResponse::Ptr HsmService::make_request(
    const HsmServiceRequest& req, hestia::Stream* stream) noexcept
{
    switch (req.method()) {
        case HsmServiceRequestMethod::GET:
            return get(req, stream);
        case HsmServiceRequestMethod::PUT:
            return put(req, stream);
        case HsmServiceRequestMethod::EXISTS:
            return exists(req);
        case HsmServiceRequestMethod::COPY:
            return copy(req);
        case HsmServiceRequestMethod::MOVE:
            return move(req);
        case HsmServiceRequestMethod::REMOVE:
            return remove(req);
        case HsmServiceRequestMethod::REMOVE_ALL:
            return remove_all(req);
        case HsmServiceRequestMethod::GET_TIERS:
        case HsmServiceRequestMethod::LIST:
        case HsmServiceRequestMethod::LIST_TIERS:
        default:
            return nullptr;
    }
}

HsmServiceResponse::Ptr HsmService::put(
    const HsmServiceRequest& req, hestia::Stream* stream) noexcept
{
    LOG_INFO("Starting HSMService PUT: " + req.to_string());

    auto exists = m_store->exists(req.object());
    ERROR_CHECK(exists);
    if (exists->object_found() != req.should_overwrite_put()) {
        const std::string msg =
            "Overwrite request not compatible with object existence.";
        ON_ERROR(BAD_PUT_OVERWRITE_COMBINATION, msg);
    }

    if (stream != nullptr) {
        const auto chosen_tier = m_data_placement_engine->choose_tier(
            req.extent().m_length, req.target_tier());
        auto data_put_response = m_store->put_data(
            req.object(), req.extent(), req.source_tier(), chosen_tier,
            req.should_overwrite_put(), stream);
        ERROR_CHECK(data_put_response);

        // Update metadata
        auto obj = req.object();
        HsmObject hsm_object(obj);
        hsm_object.add_tier(chosen_tier);

        m_object_adapter->sync(hsm_object, obj);
        auto metadata_put_response = m_store->put_metadata(obj);
        auto response =
            HsmServiceResponse::create(req, std::move(metadata_put_response));

        LOG_INFO("Finished HSMService PUT");
        return response;
    }

    if (!req.query().empty()) {
        HsmAction action;
        m_action_adapter->parse(req.query(), action);
        if (action.has_action()) {
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

    auto metadata_put_response = m_store->put_metadata(req.object());
    auto response =
        HsmServiceResponse::create(req, std::move(metadata_put_response));
    LOG_INFO("Finished HSMService PUT");
    return response;
}

HsmServiceResponse::Ptr HsmService::get(
    const HsmServiceRequest& req, hestia::Stream* stream) noexcept
{
    LOG_INFO("Starting HSMService GET: " + req.to_string());

    auto exists = m_store->exists(req.object());
    ERROR_CHECK(exists);
    if (!exists->object_found()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    if (stream != nullptr) {
        auto response = m_store->get_data(
            req.object(), req.extent(), req.source_tier(), stream);
        LOG_INFO("Finished HSMService GET");
        return HsmServiceResponse::create(req, std::move(response));
    }
    else {
        auto response = m_store->get_metadata(req.object());
        LOG_INFO("Finished HSMService GET");
        return HsmServiceResponse::create(req, std::move(response));
    }
}

HsmServiceResponse::Ptr HsmService::exists(
    const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService EXISTS: " + req.to_string());

    auto exists = m_store->exists(req.object());
    ERROR_CHECK(exists);
    if (!exists->object_found()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }
    else {
        return HsmServiceResponse::create(req, std::move(exists));
    }
}

HsmServiceResponse::Ptr HsmService::copy(const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService COPY: " + req.to_string());

    auto exists = m_store->exists(req.object());
    ERROR_CHECK(exists);
    if (!exists->object_found()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    auto obj = req.object();
    /*
    m_key_value_store->fetch(obj, {"tiers"});
    */

    HsmObject hsm_object(obj);

    m_object_adapter->parse_tiers(hsm_object);

    auto copy_data_response = m_store->copy_data(
        req.object(), req.extent(), req.source_tier(), req.target_tier());
    ERROR_CHECK(copy_data_response);
    LOG_INFO("Finished HSMService COPY - COPY DATA");

    hsm_object.add_tier(req.target_tier());
    m_object_adapter->serialize(hsm_object);
    LOG_INFO("Finished HSMService COPY - ADD TIER");

    auto metadata_put_response = m_store->put_metadata(obj);
    LOG_INFO("Finished HSMService COPY - PUT METADATA");
    return HsmServiceResponse::create(req, std::move(metadata_put_response));
}

HsmServiceResponse::Ptr HsmService::move(const HsmServiceRequest& req) noexcept
{
    LOG_INFO("Starting HSMService MOVE: " + req.to_string());

    auto exists = m_store->exists(req.object());
    ERROR_CHECK(exists);
    if (!exists->object_found()) {
        const std::string msg = "Requested Object Not Found.";
        ON_ERROR(OBJECT_NOT_FOUND, msg);
    }

    auto obj = req.object();
    /*
    m_key_value_store->fetch(obj, {"tiers"});
    */

    HsmObject hsm_object(obj);

    m_object_adapter->parse_tiers(hsm_object);

    auto move_data_response = m_store->move_data(
        req.object(), req.extent(), req.source_tier(), req.target_tier());
    ERROR_CHECK(move_data_response);
    LOG_INFO("Finished HSMService MOVE - MOVE DATA");

    hsm_object.add_tier(req.target_tier());
    m_object_adapter->serialize(hsm_object);
    LOG_INFO("Finished HSMService MOVE - ADD TIER");

    auto metadata_put_response = m_store->put_metadata(obj);
    LOG_INFO("Finished HSMService MOVE - PUT METADATA");
    return HsmServiceResponse::create(req, std::move(metadata_put_response));
}

HsmServiceResponse::Ptr HsmService::remove(
    const HsmServiceRequest& req) noexcept
{
    auto obj = req.object();

    /*
    m_key_value_store->fetch(obj, {"tiers"});
    */

    HsmObject hsm_object(obj);
    m_object_adapter->parse_tiers(hsm_object);
    m_object_adapter->serialize(hsm_object);

    auto remove_data_response =
        m_store->release_data(req.object(), req.extent(), req.source_tier());
    ERROR_CHECK(remove_data_response);

    hsm_object.remove_tier(req.source_tier());
    m_object_adapter->serialize(hsm_object);

    auto metadata_put_response = m_store->put_metadata(obj);
    return HsmServiceResponse::create(req, std::move(metadata_put_response));
}

HsmServiceResponse::Ptr HsmService::remove_all(
    const HsmServiceRequest& req) noexcept
{
    auto obj = req.object();

    /*
    m_key_value_store->fetch(obj, {"tiers"});
    */

    HsmObject hsm_object(obj);
    m_object_adapter->parse_tiers(hsm_object);
    m_object_adapter->serialize(hsm_object);

    auto remove_data_response =
        m_store->release_data(req.object(), req.extent());
    ERROR_CHECK(remove_data_response);

    hsm_object.remove_all_but_one_tiers();
    m_object_adapter->serialize(hsm_object);

    auto metadata_put_response = m_store->put_metadata(obj);
    return HsmServiceResponse::create(req, std::move(metadata_put_response));
}

void HsmService::list_objects(uint8_t tier, std::vector<HsmObject>& objects)
{
    (void)tier;
    (void)objects;
    /*
    std::vector<hestia::StorageObject> objs;
    m_key_value_store->list(tier, objs);
    /*/
}

void HsmService::list_tiers(HsmObject& object, std::vector<uint8_t>& tiers)
{
    (void)tiers;
    (void)object;
    /*
    if (!m_key_value_store->exists(object.mStorageObject))
    {
        throw std::runtime_error("Requested non-existing object");
    }

    m_key_value_store->fetch(object.mStorageObject, {"tiers"});
    HsmObjectAdapter::parseTiers(object);
    tiers = object.mTierIds;
    */
}

void HsmService::list_attributes(HsmObject& object)
{
    (void)object;
}

}  // namespace hestia
