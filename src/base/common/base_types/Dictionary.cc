#include "Dictionary.h"

namespace hestia {
Dictionary::Dictionary(Dictionary::Type type) : m_type(type) {}

Dictionary::Ptr Dictionary::create(Dictionary::Type type)
{
    return std::make_unique<Dictionary>(type);
}

void Dictionary::add_sequence_item(std::unique_ptr<Dictionary> item)
{
    m_sequence.push_back(std::move(item));
}

void Dictionary::for_each_scalar(onItem func) const
{
    if (m_type == Type::MAP) {
        for (const auto& [key, value] : m_map) {
            if (value->get_type() == Type::SCALAR) {
                func(key, value->get_scalar());
            }
        }
    }
}

Dictionary* Dictionary::get_map_item(const std::string& key) const
{
    if (auto iter = m_map.find(key); iter != m_map.end()) {
        return iter->second.get();
    }
    return nullptr;
}

Dictionary::Type Dictionary::get_type() const
{
    return m_type;
}

const std::string& Dictionary::get_scalar() const
{
    return m_scalar;
}

bool Dictionary::has_map_item(const std::string& key) const
{
    auto iter = m_map.find(key);
    return iter != m_map.end();
}

const std::vector<std::unique_ptr<Dictionary>>& Dictionary::get_sequence() const
{
    return m_sequence;
}

void Dictionary::set_map_item(
    const std::string& key, std::unique_ptr<Dictionary> item)
{
    m_map[key] = std::move(item);
}

void Dictionary::set_map(
    const std::unordered_map<std::string, std::string>& items)
{
    for (const auto& [key, value] : items) {
        auto dict = Dictionary::create(Dictionary::Type::SCALAR);
        dict->set_scalar(value);
        set_map_item(key, std::move(dict));
    }
}

void Dictionary::set_scalar(const std::string& scalar)
{
    m_scalar = scalar;
}
}  // namespace hestia