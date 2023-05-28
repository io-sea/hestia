#include "Dictionary.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

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

void Dictionary::get_map_items(
    Metadata& sink, const std::vector<std::string>& exclude_keys) const
{
    if (m_type == Type::MAP) {
        for (const auto& [key, value] : m_map) {
            if (value->get_type() == Type::SCALAR
                && std::find(exclude_keys.begin(), exclude_keys.end(), key)
                       == exclude_keys.end()) {
                sink.set_item(key, value->get_scalar());
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

const std::unordered_map<std::string, std::unique_ptr<Dictionary>>&
Dictionary::get_map() const
{
    return m_map;
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


bool Dictionary::has_tag() const
{
    return !m_tag.second.empty();
}

const std::pair<std::string, std::string>& Dictionary::get_tag() const
{
    return m_tag;
}

void Dictionary::set_tag(const std::string& tag, const std::string& prefix)
{
    m_tag.first.assign(prefix);
    m_tag.second.assign(tag);
}

bool Dictionary::is_empty() const
{
    switch (m_type) {
        case Type::MAP:
            return m_map.empty();
        case Type::SEQUENCE:
            return m_sequence.empty();
        case Type::SCALAR:
            return m_scalar.empty();
    }
    return true;
}

bool Dictionary::operator==(const Dictionary& rhs) const
{
    if (m_type != rhs.get_type()) {
        return false;
    }

    if (m_tag != rhs.get_tag()) {
        return false;
    }

    if (m_type == Dictionary::Type::MAP) {
        for (const auto& [key, value] : m_map) {
            if (!rhs.has_map_item(key)) {
                return false;
            }
            if (value == nullptr || rhs.get_map_item(key) == nullptr) {
                if (value != nullptr || rhs.get_map_item(key) != nullptr) {
                    return false;
                }
            }
            if (value != nullptr) {
                if (*value == *(rhs.get_map_item(key))) {
                    return false;
                }
            }
        }
    }
    else if (m_type == Dictionary::Type::SEQUENCE) {
        if (m_sequence.size() != rhs.get_sequence().size()) {
            return false;
        }

        for (size_t i = 0; i < m_sequence.size(); i++) {
            if (!(*(m_sequence[i]) == *(rhs.get_sequence()[i]))) {
                return false;
            }
        }
    }
    else if (m_type == Dictionary::Type::SCALAR) {
        return m_scalar == rhs.get_scalar();
    }

    return true;
}

}  // namespace hestia