#include "DictField.h"

namespace hestia {
ScalarMapField::ScalarMapField(const std::string& name) : DictField(name)
{
    m_type = "scalar_map";
}

void ScalarMapField::serialize(Dictionary& dict, Format) const
{
    dict.set_map(m_scalar_map.data());
}

void ScalarMapField::deserialize(const Dictionary& dict, Format)
{
    dict.get_map_items(m_scalar_map);
}

const Map& ScalarMapField::get_map() const
{
    return m_scalar_map;
}

Map& ScalarMapField::get_map_as_writeable()
{
    m_modified = true;
    return m_scalar_map;
}

void ScalarMapField::set_map_item(
    const std::string& key, const std::string& value)
{
    m_scalar_map.set_item(key, value);
    m_modified = true;
}

RawDictField::RawDictField(const std::string& name) : DictField(name) {}

const Dictionary& RawDictField::value() const
{
    return m_value;
}

Dictionary& RawDictField::get_value_as_writeable()
{
    m_modified = true;
    return m_value;
}

void RawDictField::update_value(const Dictionary& value)
{
    m_value = value;
}

void RawDictField::serialize(Dictionary& dict, Format) const
{
    dict = m_value;
}

void RawDictField::deserialize(const Dictionary& dict, Format)
{
    m_value = dict;
}

}  // namespace hestia
