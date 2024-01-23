#include "CrudIdentifierCollection.h"

namespace hestia {

CrudIdentifierCollection::CrudIdentifierCollection(
    const std::vector<std::string>& primary_keys)
{
    load_primary_keys(primary_keys);
}

CrudIdentifierCollection::CrudIdentifierCollection(
    const std::vector<std::string>& keys,
    const CrudIdentifier::InputFormat& input_format)
{
    for (const auto& key : keys) {
        CrudIdentifier::FormatSpec format;
        format.m_input_format = input_format;
        m_ids.emplace_back(key, format);
    }
}

void CrudIdentifierCollection::load_primary_keys(
    const std::vector<std::string>& primary_keys)
{
    for (const auto& key : primary_keys) {
        m_ids.emplace_back(key);
    }
}

void CrudIdentifierCollection::get_primary_keys(
    std::vector<std::string>& keys) const
{
    for (const auto& item : m_ids) {
        keys.push_back(item.get_primary_key());
    }
}

void CrudIdentifierCollection::load(
    const Dictionary& dict, const CrudIdentifier::FormatSpec& format)
{
    if (dict.is_sequence()) {
        for (const auto& item : dict.get_sequence()) {
            if (item->has_map_item(format.m_primary_key_name)) {
                m_ids.emplace_back(item->get_map_item(format.m_primary_key_name)
                                       ->get_scalar());
            }
        }
    }
    else {
        if (dict.has_map_item(format.m_primary_key_name)) {
            m_ids.emplace_back(
                dict.get_map_item(format.m_primary_key_name)->get_scalar());
        }
    }
}

CrudIdentifierCollection::CrudIdentifierCollection(
    const std::string& buffer, const FormatSpec& format)
{
    load(buffer, format);
}

void CrudIdentifierCollection::append(const CrudIdentifierCollection& other)
{
    for (const auto& id : other.m_ids) {
        m_ids.push_back(id);
    }
}

const CrudIdentifier& CrudIdentifierCollection::first() const
{
    if (m_ids.size() != 1) {
        throw std::runtime_error("Single crud id requested but none set");
    }
    return m_ids[0];
}

const CrudIdentifier& CrudIdentifierCollection::id(std::size_t index) const
{
    if (index >= m_ids.size()) {
        throw std::runtime_error("Out of bounds access in id collection.");
    }
    return m_ids[index];
}

bool CrudIdentifierCollection::empty() const
{
    return m_ids.empty();
}

const VecCrudIdentifier& CrudIdentifierCollection::data() const
{
    return m_ids;
}

std::size_t CrudIdentifierCollection::size() const
{
    return m_ids.size();
}

void CrudIdentifierCollection::add(const CrudIdentifier& id)
{
    m_ids.push_back(id);
}

void CrudIdentifierCollection::add_primary_key(const std::string& key)
{
    m_ids.emplace_back(key);
}

void CrudIdentifierCollection::add_primary_keys(
    const std::vector<std::string>& keys)
{
    for (const auto& key : keys) {
        add_primary_key(key);
    }
}

void CrudIdentifierCollection::write(
    Dictionary& dict, const CrudIdentifier::FormatSpec& format) const
{
    dict.set_type(Dictionary::Type::SEQUENCE);
    for (const auto& id : m_ids) {
        auto seq_item = Dictionary::create();
        id.write(*seq_item, format);
        dict.add_sequence_item(std::move(seq_item));
    }
}

void CrudIdentifierCollection::write(
    std::string& buffer,
    const CrudIdentifierCollection::FormatSpec& format) const
{
    std::size_t count{0};
    for (const auto& id : m_ids) {
        buffer += id.get_primary_key();
        if (count < m_ids.size() - 1) {
            buffer += format.m_id_delimiter;
        }
        count++;
    }
}

void CrudIdentifierCollection::load(
    const std::vector<std::string>& buffer,
    const CrudIdentifier::FormatSpec& format)
{
    for (const auto& entry : buffer) {
        m_ids.emplace_back(entry, format);
    }
}

std::size_t CrudIdentifierCollection::load(
    const std::string& buffer, const FormatSpec& format)
{
    if (format.m_id_spec.m_input_format == CrudIdentifier::InputFormat::NONE) {
        return 0;
    }

    const auto& [id_section, _] =
        StringUtils::split_on_first(buffer, format.m_end_delimiter);
    if (id_section.empty()) {
        return 0;
    }

    std::vector<std::string> entries;
    StringUtils::split(id_section, format.m_id_delimiter, entries);
    load(entries, format.m_id_spec);

    return id_section.size();
}
}  // namespace hestia