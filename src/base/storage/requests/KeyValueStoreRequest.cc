#include "KeyValueStoreRequest.h"

namespace hestia {
KeyValueStoreRequest::KeyValueStoreRequest(
    KeyValueStoreRequestMethod method, const Metadata::Query& query) :
    MethodRequest<KeyValueStoreRequestMethod>(method),
    BaseRequest(),
    m_query(query)
{
}

const Metadata::Query& KeyValueStoreRequest::get_query() const
{
    return m_query;
}

std::string KeyValueStoreRequest::method_as_string() const
{
    switch (m_method) {
        case KeyValueStoreRequestMethod::STRING_EXISTS:
            return "STRING_EXISTS";
        case KeyValueStoreRequestMethod::STRING_GET:
            return "STRING_GET";
        case KeyValueStoreRequestMethod::STRING_SET:
            return "STRING_SET";
        case KeyValueStoreRequestMethod::STRING_REMOVE:
            return "STRING_REMOVE";
        case KeyValueStoreRequestMethod::SET_ADD:
            return "SET_ADD";
        case KeyValueStoreRequestMethod::SET_LIST:
            return "SET_LIST";
        case KeyValueStoreRequestMethod::SET_REMOVE:
            return "SET_REMOVE";
        default:
            return "UNKNOWN";
    }
}

}  // namespace hestia