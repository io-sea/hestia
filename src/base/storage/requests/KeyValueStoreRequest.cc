#include "KeyValueStoreRequest.h"

namespace hestia {
KeyValueStoreRequest::KeyValueStoreRequest(
    KeyValueStoreRequestMethod method,
    const Metadata::Query& query,
    const std::string& url) :
    MethodRequest<KeyValueStoreRequestMethod>(method),
    BaseRequest(url),
    m_query(query)
{
}

KeyValueStoreRequest::KeyValueStoreRequest(
    KeyValueStoreRequestMethod method,
    const std::string& key,
    const std::string& url) :
    MethodRequest<KeyValueStoreRequestMethod>(method),
    BaseRequest(url),
    m_key(key)
{
}

KeyValueStoreRequest::KeyValueStoreRequest(
    KeyValueStoreRequestMethod method,
    const std::vector<std::string>& keys,
    const std::string& url) :
    MethodRequest<KeyValueStoreRequestMethod>(method),
    BaseRequest(url),
    m_keys(keys)
{
}

const Metadata::Query& KeyValueStoreRequest::get_query() const
{
    return m_query;
}

const std::string& KeyValueStoreRequest::get_key() const
{
    return m_key;
}

const std::vector<std::string>& KeyValueStoreRequest::get_keys() const
{
    return m_keys;
}

std::string KeyValueStoreRequest::method_as_string() const
{
    switch (m_method) {
        case KeyValueStoreRequestMethod::STRING_EXISTS:
            return "STRING_EXISTS";
        case KeyValueStoreRequestMethod::STRING_GET:
            return "STRING_GET";
        case KeyValueStoreRequestMethod::STRING_MULTI_GET:
            return "STRING_MULTI_GET";
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