#include "KeyValueStoreResponse.h"

namespace hestia {
KeyValueStoreResponse::KeyValueStoreResponse(
    const KeyValueStoreRequest& request) :
    Response<KeyValueStoreErrorCode>(request)
{
}

KeyValueStoreResponse::Ptr KeyValueStoreResponse::create(
    const KeyValueStoreRequest& request)
{
    return std::make_unique<KeyValueStoreResponse>(request);
}

const std::vector<std::string>& KeyValueStoreResponse::get_set_items() const
{
    return m_set_items;
}

std::vector<std::string>& KeyValueStoreResponse::get_set_items()
{
    return m_set_items;
}

bool KeyValueStoreResponse::has_key() const
{
    return m_key_found;
}

void KeyValueStoreResponse::set_key_found(bool found)
{
    m_key_found = found;
}

std::string& KeyValueStoreResponse::get_value()
{
    return m_value;
}

const std::string& KeyValueStoreResponse::get_value() const
{
    return m_value;
}
}  // namespace hestia