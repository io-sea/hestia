#include "KeyValueStoreResponse.h"

namespace hestia {
KeyValueStoreResponse::KeyValueStoreResponse(const BaseRequest& request) :
    Response<CrudErrorCode>(request)
{
}

KeyValueStoreResponse::~KeyValueStoreResponse() {}

const std::vector<std::vector<std::string>>& KeyValueStoreResponse::ids() const
{
    return m_ids;
}

std::vector<std::vector<std::string>>& KeyValueStoreResponse::ids()
{
    return m_ids;
}

const std::vector<std::string>& KeyValueStoreResponse::get_items() const
{
    return m_items;
}

std::vector<std::string>& KeyValueStoreResponse::items()
{
    return m_items;
}

const std::vector<bool>& KeyValueStoreResponse::found() const
{
    return m_found;
}

std::vector<bool>& KeyValueStoreResponse::found()
{
    return m_found;
}

void KeyValueStoreResponse::set_locked(bool is_locked)
{
    m_locked = is_locked;
}

bool KeyValueStoreResponse::locked() const
{
    return m_locked;
}
}  // namespace hestia