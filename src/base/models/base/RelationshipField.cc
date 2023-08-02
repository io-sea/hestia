#include "RelationshipField.h"

namespace hestia {

ForeignKeyField::ForeignKeyField(
    const std::string& name, const std::string& type, bool is_parent) :
    DictField(name), m_is_parent(is_parent)
{
    m_type = type;
}

void ForeignKeyField::serialize(Dictionary& dict, Format format) const
{
    (void)format;
    dict.set_map({{m_uuid.get_name(), m_uuid.value_as_string()}});
}

void ForeignKeyField::deserialize(const Dictionary& dict, Format format)
{
    (void)format;
    if (dict.has_map_item(m_uuid.get_name())) {
        m_uuid.value_from_string(
            dict.get_map_item(m_uuid.get_name())->get_scalar());
    }
}
bool ForeignKeyField::is_parent() const
{
    return m_is_parent;
}

const std::string& ForeignKeyField::get_id() const
{
    return m_uuid.get_value();
}

void ForeignKeyField::set_id(const std::string& id)
{
    m_uuid.update_value(id);
}


ManyToManyField::ManyToManyField(
    const std::string& name, const std::string& type) :
    DictField(name)
{
    m_type = type;
}

void ManyToManyField::serialize(Dictionary& dict, Format format) const
{
    auto sequence_dict =
        std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    m_uuids.serialize(*sequence_dict, format);
    dict.set_map_item(m_uuids.get_name(), std::move(sequence_dict));
}

void ManyToManyField::deserialize(const Dictionary& dict, Format format)
{
    if (dict.has_map_item(m_uuids.get_name())) {
        m_uuids.deserialize(*dict.get_map_item(m_uuids.get_name()), format);
    }
}

const std::vector<std::string>& ManyToManyField::get_ids() const
{
    return m_uuids.container();
}

void ManyToManyField::set_ids(const std::vector<std::string>& ids)
{
    m_uuids.get_container_as_writeable() = ids;
}

}  // namespace hestia