#include "KeyValueFieldContext.h"

#include "ErrorUtils.h"

#include <cassert>

namespace hestia {

KeyValueFieldContext::KeyValueFieldContext(
    const AdapterCollection* adapters, const std::string& key_prefix) :
    m_adapters(adapters), m_key_prefix(key_prefix)
{
}

std::string KeyValueFieldContext::get_item_key(const std::string& id) const
{
    return get_prefix() + ":" + id;
}

std::string KeyValueFieldContext::get_field_key(
    const std::string& field, const std::string& value) const
{
    return get_prefix() + "_" + field + ":" + value;
}

void KeyValueFieldContext::get_item_keys(
    const std::vector<std::string>& ids, std::vector<std::string>& keys) const
{
    for (const auto& id : ids) {
        keys.push_back(get_prefix() + ":" + id);
    }
}

std::string KeyValueFieldContext::get_prefix() const
{
    return m_key_prefix + ":" + m_adapters->get_type();
}

std::string KeyValueFieldContext::get_set_key() const
{
    return get_prefix() + "s";
}

}  // namespace hestia