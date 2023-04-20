#include "HestiaCppInterface.h"

#include "hestia.h"

#include "ApplicationContext.h"
#include "HsmService.h"

#include <ostk/InMemoryStreamSink.h>
#include <ostk/InMemoryStreamSource.h>

namespace rc{
int error(const ostk::RequestError<HsmServiceErrorCode>& error)
{
    switch(error.code())
    {
        case HsmServiceErrorCode::NO_ERROR:
            return hestia::hestia_error_t::HESTIA_ERROR_OK;
        default:
            return hestia::hestia_error_t::HESTIA_ERROR_UNKOWN;
    }
}

int ok()
{
    return hestia::hestia_error_t::HESTIA_ERROR_OK;
}

int badStream()
{
    return hestia::hestia_error_t::HESTIA_ERROR_BAD_STREAM;
}

}

namespace hestia{
int HestiaCppInterface::put(const ostk::Uuid& objectId, const ostk::Extent& extent, const ostk::ReadableBufferView& buffer, 
        uint8_t tierId, bool overwrite)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::PUT);
    request.setExtent(extent);
    request.setTargetTier(tierId);
    request.setShouldPutOverwrite(overwrite);

    ostk::Stream stream;
    stream.setSource(ostk::InMemoryStreamSource::Create(buffer));

    if (const auto response = ApplicationContext::get().getHsmService()->makeRequest(request, &stream); !response->ok())
    {
        return rc::error(response->getError());
    }

    if (const auto stream_state = stream.flush(); !stream_state.ok())
    {
        return rc::badStream();
    }
    return rc::ok();
}

int HestiaCppInterface::get(const ostk::Uuid& objectId, const ostk::Extent& extent, ostk::WriteableBufferView& buffer, uint8_t tierId)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET);
    request.setExtent(extent);
    request.setSourceTier(tierId);

    ostk::Stream stream;
    stream.setSink(ostk::InMemoryStreamSink::Create(buffer));

    if (const auto response = ApplicationContext::get().getHsmService()->makeRequest(request, &stream); !response->ok())
    {
        return rc::error(response->getError());
    }

    if (const auto stream_state = stream.flush(); !stream_state.ok())
    {
        return rc::badStream();
    }
    return rc::ok();
}

int HestiaCppInterface::copy(const ostk::Uuid& objectId, const ostk::Extent& extent, uint8_t sourceTier, uint8_t targetTier)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::COPY);
    request.setExtent(extent);
    request.setSourceTier(sourceTier);
    request.setTargetTier(targetTier);

    if (const auto response = ApplicationContext::get().getHsmService()->makeRequest(request); !response->ok())
    {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::move(const ostk::Uuid& objectId, const ostk::Extent& extent, uint8_t sourceTier, uint8_t targetTier)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::MOVE);
    request.setExtent(extent);
    request.setSourceTier(sourceTier);
    request.setTargetTier(targetTier);

    if (const auto response = ApplicationContext::get().getHsmService()->makeRequest(request); !response->ok())
    {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::getAttributes(const ostk::Uuid& objectId, const std::string& keys, std::string& attributes)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET);
    request.setQuery(keys);

    const auto response = ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok())
    {
        return rc::error(response->getError());
    }

    attributes = response->queryResult();
    return rc::ok();
}

int HestiaCppInterface::setAttributes(const ostk::Uuid& objectId, const std::string& attributes)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET);
    request.setQuery(attributes);

    if (const auto response = ApplicationContext::get().getHsmService()->makeRequest(request); !response->ok())
    {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::release(const ostk::Uuid& objectId)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::REMOVE_ALL); 
    if (const auto response = ApplicationContext::get().getHsmService()->makeRequest(request); !response->ok())
    {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::release(const ostk::Uuid& objectId, uint8_t tierId, bool removeKey)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::REMOVE);
    request.setSourceTier(tierId);
    const auto response = ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok())
    {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::listObjects(uint8_t tier, std::vector<ostk::Uuid> objects)
{
    HsmServiceRequest request(HsmServiceRequestMethod::GET);
    request.setSourceTier(tier);
    const auto response = ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok())
    {
        return rc::error(response->getError());
    }
    objects = response->objects();

    return rc::ok(); 
}

int HestiaCppInterface::listTiers(const ostk::Uuid& objectId, std::vector<uint8_t>& tiers)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET_TIERS);
    const auto response = ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok())
    {
        return rc::error(response->getError());
    }
    tiers = response->tiers();
    return rc::ok(); 
}

int HestiaCppInterface::listAttributes(const ostk::Uuid& objectId, std::string& attributes)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET);
    const auto response = ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok())
    {
        return rc::error(response->getError());
    }
    attributes = response->queryResult();
    return rc::ok();    
}
}