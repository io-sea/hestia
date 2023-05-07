#include "ObjectStoreRequest.h"

namespace hestia {

ObjectStoreRequest::ObjectStoreRequest(
    const StorageObject& object, ObjectStoreRequestMethod method) :
    MethodObjectStoreRequest<ObjectStoreRequestMethod>(object, method)
{
}

ObjectStoreRequest::ObjectStoreRequest(
    const std::string& object_id, ObjectStoreRequestMethod method) :
    MethodObjectStoreRequest<ObjectStoreRequestMethod>(object_id, method)
{
}

ObjectStoreRequest::ObjectStoreRequest(const Metadata::Query& query) :
    MethodObjectStoreRequest<ObjectStoreRequestMethod>(
        query, ObjectStoreRequestMethod::LIST)
{
}

std::string ObjectStoreRequest::method_as_string() const
{
    return to_string(m_method);
}

std::string ObjectStoreRequest::to_string(ObjectStoreRequestMethod method)
{
    switch (method) {
        case ObjectStoreRequestMethod::EXISTS:
            return "EXISTS";
        case ObjectStoreRequestMethod::GET:
            return "GET";
        case ObjectStoreRequestMethod::PUT:
            return "PUT";
        case ObjectStoreRequestMethod::REMOVE:
            return "REMOVE";
        case ObjectStoreRequestMethod::LIST:
            return "LIST";
        case ObjectStoreRequestMethod::CUSTOM:
            return "CUSTOM";
        default:
            return "UNKNOWN";
    }
}

}  // namespace hestia